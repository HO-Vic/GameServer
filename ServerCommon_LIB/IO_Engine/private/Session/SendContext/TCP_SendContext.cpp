#include <pch.h>
#include <Session/SendContext/TCP_SendContext.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBufferPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <Utility/Thread/IWorkerItem.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
int32_t TCP_SendContext::DoSend(Utility::WorkerPtr session, std::shared_ptr<SendBufferBase>&& buffer, SendPolicy policy) {
  {
    std::lock_guard<std::mutex> lg{m_lock};
    m_doubleQueue.InsertSendBuffer(std::move(buffer));
    if (policy == SendPolicy::Deferred || !m_isSendAble) { // 다른 스레드가 송신 중. 위에서 push한 버퍼는 다음 SendComplete의 SwapBuffer에서 같이 송신됨.
      return 0;
    }
    // push + claim + swap이 같은 critical section. SendComplete의 release와 race 없음.
    m_isSendAble = false;
    m_doubleQueue.SwapBuffer();
  }
  Utility::ThWorkerJob* workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::SEND);
  // claim 보유 중이므로 Consume 버퍼는 안정. 직전 push로 비어있지 않음.
  auto errorNo = SendExecute(workerJob, m_doubleQueue.Consume());
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

void TCP_SendContext::PushBatch(std::vector<std::shared_ptr<SendBufferBase>>&& buffers) {
  if (buffers.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lg{m_lock};
  m_doubleQueue.InsertBatch(std::move(buffers));
}

int32_t TCP_SendContext::Flush(Utility::WorkerPtr session) {
  Utility::ThWorkerJob* workerJob = nullptr;
  {
    std::lock_guard<std::mutex> lg{m_lock};
    if (!m_isSendAble) {
      return 0;
    }
    m_doubleQueue.SwapBuffer();
    if (m_doubleQueue.Consume().empty()) {
      // 빈 swap. claim 안 함. idx flip은 다음 cycle에서 자연스레 회복.
      return 0;
    }
    m_isSendAble = false;
    workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::SEND);
  }
  auto errorNo = SendExecute(workerJob, m_doubleQueue.Consume());
  if (0 != errorNo) {
    auto ioError = WSAGetLastError();
    if (WSA_IO_PENDING == ioError) {
      errorNo = 0;
    } else {
      errorNo = ioError;
      ThWorkerJobPool::GetInstance().Release(workerJob);
    }
  }
  return errorNo;
}

int32_t TCP_SendContext::SendComplete(Utility::ThWorkerJob* workerJob, const size_t ioByte) {
  // 직전 송신분 해제
  m_sendBuffer.clear();

  {
    std::lock_guard<std::mutex> lg{m_lock};
    m_doubleQueue.SwapBuffer();
    if (m_doubleQueue.Consume().empty()) {
      // 같은 critical section에서 송신권 release.
      // 직후 다른 스레드의 InsertSendBuffer가 들어오면 그쪽이 claim 가능 → race window 없음.
      m_isSendAble = true;
      ThWorkerJobPool::GetInstance().Release(workerJob);
      return 0;
    }
    // 보낼 게 있으면 m_isSendAble은 false 유지(연속 송신)
  }

  auto errorNo = SendExecute(workerJob, m_doubleQueue.Consume());
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

int32_t TCP_SendContext::SendExecute(Utility::ThWorkerJob* workerJob, std::vector<std::shared_ptr<SendBufferBase>>& batch) {
  // 호출자가 batch 비어있지 않음을 보장 (lock 안 empty 체크 후 호출)

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

  m_sendBuffer.swap(batch);
  m_wsaBuffers.clear();
  m_wsaBuffers.reserve(m_sendBuffer.size());
  for (const auto& sendBuffer : m_sendBuffer) {
    m_wsaBuffers.push_back(sendBuffer->GetWSABuffer());
  }
  return WSASend(m_socket, m_wsaBuffers.data(), static_cast<DWORD>(m_wsaBuffers.size()), nullptr, 0, reinterpret_cast<LPOVERLAPPED>(workerJob), nullptr);
}

void TCP_SendContext::InternalDoubleBufferQueue::InsertSendBuffer(std::shared_ptr<SendBufferBase>&& buffer) {
  // caller holds m_lock
  m_sendQueues[m_activeIdx].push_back(std::move(buffer));
}

void TCP_SendContext::InternalDoubleBufferQueue::InsertBatch(std::vector<std::shared_ptr<SendBufferBase>>&& buffers) {
  // caller holds m_lock
  if (buffers.empty()) {
    return;
  }
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

void TCP_SendContext::InternalDoubleBufferQueue::SwapBuffer() {
  // caller holds m_lock
  m_activeIdx = !m_activeIdx;
}

std::vector<std::shared_ptr<SendBufferBase>>& TCP_SendContext::InternalDoubleBufferQueue::Consume() {
  return m_sendQueues[!m_activeIdx];
}
}  // namespace sh::IO_Engine
