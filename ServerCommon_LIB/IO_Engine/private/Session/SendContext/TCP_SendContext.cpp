#include <pch.h>
#include <Session/SendContext/TCP_SendContext.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBufferPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <Utility/Thread/IWorkerItem.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
int32_t TCP_SendContext::DoSend(Utility::WorkerPtr session, std::shared_ptr<SendBufferBase>&& buffer, SendPolicy policy) {
  m_doubleQueue.InsertSendBuffer(std::move(buffer));
  if (policy == SendPolicy::Deferred) {
    return 0;
  }
  return TryFlush(std::move(session));
}

void TCP_SendContext::PushBatch(std::vector<std::shared_ptr<SendBufferBase>>&& buffers) {
  m_doubleQueue.InsertBatch(std::move(buffers));
}

int32_t TCP_SendContext::Flush(Utility::WorkerPtr session) {
  return TryFlush(std::move(session));
}

int32_t TCP_SendContext::TryFlush(Utility::WorkerPtr session) {
  static constexpr bool SEND_DESIRE = false;
  if (!m_isSendAble) {
    return 0;
  }

  bool expectedValue = true;
  // Send가 다중 쓰레드에서 호출되더라도, 하나의 Send 시도 쓰레드만 Send
  // 1. 위에서 true였지만, 이미 false가 될 수 도 있고
  // 2. true를 유지했다면, 타 쓰레드가 변경 성공 시 포기
  bool isSendAbleThread = m_isSendAble.compare_exchange_strong(expectedValue, SEND_DESIRE);
  if (!isSendAbleThread) {
    return 0;
  }

  auto workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::SEND);
  auto errorNo = SendExecute(workerJob, m_doubleQueue.SwapAndLoad());
  if (0 != errorNo) {
    auto ioError = WSAGetLastError();
    if (WSA_IO_PENDING == ioError) {
      errorNo = 0;
    } else {
      errorNo = ioError;
      ThWorkerJobPool::GetInstance().Release(workerJob);  // SendErr났을 때, workJob을 다시 반납해야 됨
    }
  }
  return errorNo;
}

int32_t TCP_SendContext::SendComplete(Utility::ThWorkerJob* workerJob, const size_t ioByte) {
  // 여기서는 보낼게 없을 때만, sendAble을 변경하고
  // 보낼게 있다면 그대로 상태 유지
  m_sendBuffer.clear();
  auto& batchSendBuffers = m_doubleQueue.SwapAndLoad();
  if (batchSendBuffers.empty()) {
    m_isSendAble = true;
    // **4/13 25ff04d커밋에서 옛날 잔재 주석** 보낼게 없다면 overlappedEx 반납
    // 이전 코드에서는 lock_guard scope안에서 return했다가, shared_ptr<TCP_SessionBase >::strong ref 1->0
    // ~TCP_SessionBase() 호출 -> ~TCP_SendContext() -> m_queueLock invalid -> unlock 크래시 발생
    // lock scope 외부에서 해제
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return 0;
  }

  auto errorNo = SendExecute(workerJob, batchSendBuffers);
  if (0 != errorNo) {
    auto ioError = WSAGetLastError();
    if (WSA_IO_PENDING == ioError) {
      errorNo = 0;
    } else {
      errorNo = ioError;
    }
  }
  return errorNo;
}

int32_t TCP_SendContext::SendExecute(Utility::ThWorkerJob* workerJob, std::vector<std::shared_ptr<SendBufferBase>>& batchSendBuffers) {
  // this thread context switched + other thread already sended queue!!
  // DoSend() sendQeuue.push() -> thread sleep ..th1
  // DoSend() -> CAS -> Send -> SendCompletion -> sendQeue.empty() ..th2(th1에서 push한 버퍼도 송신)
  // th1 wake -> CAS -> send -> send Queue Empty -> m_isSendAble=true

  // lf-queue에 대한 코드 입니다.(지금은 안씀)
  /*auto queueSize = m_sendQueue.unsafe_size();
  for (auto i = 0; i < queueSize; ++i) {
    std::shared_ptr<SendBuffer> currentBuf = nullptr;
    bool isSuccess = m_sendQueue.try_pop(currentBuf);
    if (!isSuccess) {
      break;
    }
    m_sendBuffer.push_back(std::move(currentBuf));
  }*/

  // m_sendBuffer는 SendComplete에서 clear()
  // m_sendBuffer는 빈 vector이고, batchSendBuffers는 전송 버퍼들이 있었지만, 교환하여
  if (batchSendBuffers.empty()) {  // 69번줄 설명의 콘텍스트 스위칭으로 인해서 97번줄 WSASend()에서 3번째 인자 0들어가서 오류 발생
    m_isSendAble = true;           // 그래서 여기서 한 번 더 확인하는 절차 추가
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return 0;
  }

  m_sendBuffer.swap(batchSendBuffers);
  m_wsaBuffers.clear();
  m_wsaBuffers.reserve(m_sendBuffer.size());
  for (const auto& sendBuffer : m_sendBuffer) {
    m_wsaBuffers.push_back(sendBuffer->GetWSABuffer());
  }
  return WSASend(m_socket, m_wsaBuffers.data(), static_cast<DWORD>(m_wsaBuffers.size()), nullptr, 0, reinterpret_cast<LPOVERLAPPED>(workerJob), nullptr);
}

void TCP_SendContext::InternalDoubleBufferQueue::InsertSendBuffer(std::shared_ptr<SendBufferBase>&& buffer) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_sendQueues[m_activeIdx].push_back(std::move(buffer));
}

void TCP_SendContext::InternalDoubleBufferQueue::InsertBatch(std::vector<std::shared_ptr<SendBufferBase>>&& buffers) {
  if (buffers.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lg{m_lock};
  auto& active = m_sendQueues[m_activeIdx];
  if (active.empty()) {
    active = std::move(buffers);
    return;
  }
  active.reserve(active.size() + buffers.size());
  active.insert(active.end(),
                std::make_move_iterator(buffers.begin()),
                std::make_move_iterator(buffers.end()));
}

std::vector<std::shared_ptr<SendBufferBase>>& TCP_SendContext::InternalDoubleBufferQueue::SwapAndLoad() {
  {
    std::lock_guard<std::mutex> lg{m_lock};
    m_activeIdx = !m_activeIdx;
  }
  return m_sendQueues[!m_activeIdx];
}
}  // namespace sh::IO_Engine