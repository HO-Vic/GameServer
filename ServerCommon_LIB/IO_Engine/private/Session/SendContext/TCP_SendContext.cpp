#include <pch.h>
#include <Session/SendContext/TCP_SendContext.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBufferPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <Utility/Thread/IWorkerItem.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
int32_t TCP_SendContext::DoSend(Utility::WorkerPtr session, const BYTE* data, const uint32_t len) {
  // thWorker가 내부에서만 존재하니, 내부에서 해결
  static constexpr bool SEND_DESIRE = false;
  auto sendData = SendBufferPool::GetInstance().MakeShared(data, len);
  m_sendQueue.push(std::move(sendData));

  if (!m_isSendAble) {
    return 0;
  }

  bool expectedValue = true;
  // Send가 다중 쓰레드에서 호출되더라도, 하나의 Send 시도 쓰레드만 Send
  // 1. 16번 라인에서 true였지만, 이미 false가 될 수 도 있고
  // 2. true를 유지했다면, 타 쓰레드가 변경 성공 시 포기
  bool isSendAbleThread = m_isSendAble.compare_exchange_strong(expectedValue, SEND_DESIRE);
  if (isSendAbleThread) {
    auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::SEND);
    auto errorNo = SendExecute(thWorkerJob);
    if (0 != errorNo) {
      auto ioError = WSAGetLastError();
      if (WSA_IO_PENDING == ioError) {
        errorNo = 0;
      } else {
        errorNo = ioError;
        ThWorkerJobPool::GetInstance().Release(thWorkerJob);  // SendErr났을 때, workJob을 다시 반납해야 됨
      }
    }
    return errorNo;
  }
  return 0;
}

int32_t TCP_SendContext::SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) {
  // 여기서는 보낼게 없을 때만, sendAble을 변경하고
  // 보낼게 있다면 그대로 상태 유지
  m_sendBuffer.clear();
  if (m_sendQueue.empty()) {
    m_isSendAble = true;
    // 보낼게 없다면 overlappedEx 반납
    // 이전 코드에서는 lock_guard scope안에서 return했다가, shared_ptr<TCP_ISession >::strong ref 1->0
    // ~TCP_ISession() 호출 -> ~TCP_SendContext() -> m_queueLock invalid -> unlock 크래시 발생
    // lock scope 외부에서 해제
    ThWorkerJobPool::GetInstance().Release(thWorkerJob);
    return 0;
  }

  auto errorNo = SendExecute(thWorkerJob);
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

int32_t TCP_SendContext::SendExecute(Utility::ThWorkerJob* thWorkerJob) {
  // this thread context switched + other thread already sended queue!!
  // DoSend() sendQeuue.push() -> thread sleep ..th1
  // DoSend() -> CAS -> Send -> SendCompletion -> sendQeue.empty() ..th2(th1에서 push한 버퍼도 송신)
  // th1 wake -> CAS -> send -> send Queue Empty -> m_isSendAble=true
  auto queueSize = m_sendQueue.unsafe_size();
  for (auto i = 0; i < queueSize; ++i) {
    std::shared_ptr<SendBuffer> currentBuf = nullptr;
    bool isSuccess = m_sendQueue.try_pop(currentBuf);
    if (!isSuccess) {
      break;
    }
    m_sendBuffer.push_back(std::move(currentBuf));
  }

  if (m_sendBuffer.empty()) {
    m_isSendAble = true;
    ThWorkerJobPool::GetInstance().Release(thWorkerJob);
    return 0;
  }

  std::vector<WSABUF> sendBuffers;
  sendBuffers.reserve(m_sendBuffer.size());
  for (const auto& sendBuffer : m_sendBuffer) {
    sendBuffers.push_back(
        WSABUF{
            .len = static_cast<uint32_t>(sendBuffer->m_size),
            .buf = reinterpret_cast<char*>(sendBuffer->m_buffer)});
  }
  return WSASend(m_socket, sendBuffers.data(), static_cast<DWORD>(sendBuffers.size()), nullptr, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob), nullptr);
}

void TCP_SendContext::InternalDoubleBufferQueue::InsertSendBuffer(std::shared_ptr<SendBuffer>&& buffer) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_sendQueues[m_activeIdx].push(std::move(buffer));
}

std::queue<std::shared_ptr<SendBuffer>>& TCP_SendContext::InternalDoubleBufferQueue::SwapAndLoad() {
  {
    std::lock_guard<std::mutex> lg{m_lock};
    m_activeIdx = !m_activeIdx;
  }
  return m_sendQueues[!m_activeIdx];
}
}  // namespace sh::IO_Engine