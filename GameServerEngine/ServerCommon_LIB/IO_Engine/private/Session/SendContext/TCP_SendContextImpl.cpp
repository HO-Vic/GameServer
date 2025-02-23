#include <pch.h>
#include <Session/SendContext/TCP_SendContextImpl.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBufferPool.h>
#include <IO_Core/OverlappedEx/OverlappedExPool.h>

namespace sh::IO_Engine {
int32_t TCP_SendContextImpl::DoSend(OverlappedPtr& session, const BYTE* data, const size_t len) {
  static constexpr bool SEND_DESIRE = false;
  auto sendData = SendBufferPool::GetInstance().MakeShared(data, len);
  {
    std::lock_guard<std::mutex> lg(m_queueLock);
    m_sendQueue.push(sendData);
  }

  if (!m_isSendAble) {
    return 0;
  }

  bool expectedValue = true;
  // Send가 다중 쓰레드에서 호출되더라도, 하나의 Send 시도 쓰레드만 Send
  // 1. 16번 라인에서 true였지만, 이미 false가 될 수 도 있고
  // 2. true를 유지했다면, 타 쓰레드가 변경 성공 시 포기
  bool isSendAbleThread = m_isSendAble.compare_exchange_strong(expectedValue, SEND_DESIRE);
  if (isSendAbleThread) {
    auto overlappedEx = OverlappedExPool::GetInstance().GetObjectPtr(session, OVERLAPPED_EVENT_TYPE::SEND);
    return SendExecute(overlappedEx);
  }
  return 0;
}

int32_t TCP_SendContextImpl::SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) {
  // 여기서는 보낼게 없을 때만, sendAble을 변경하고
  // 보낼게 있다면 그대로 상태 유지
  m_sendBuffer.clear();
  {
    std::lock_guard<std::mutex> lg(m_queueLock);
    if (0 == m_sendQueue.size()) {
      m_isSendAble = true;
      // 보낼게 없다면 overlappedEx 반납
      overlappedEx->m_overlappedEvent = nullptr;  // 여기서 해제를 안하게되면 오브젝트풀에서 다시 쓰여지기 전까지 이 섹션은 메모리 해제 불가
      OverlappedExPool::GetInstance().Release(overlappedEx);
      return 0;
    }
  }
  return SendExecute(overlappedEx);
}

int32_t TCP_SendContextImpl::SendExecute(OverlappedEx* overlappedEx) {
  {
    std::lock_guard<std::mutex> lg(m_queueLock);
    while (!m_sendQueue.empty()) {
      m_sendBuffer.push_back(std::move(m_sendQueue.front()));
      m_sendQueue.pop();
    }
  }
  std::vector<WSABUF> sendBuffers;
  sendBuffers.reserve(m_sendBuffer.size());
  for (const auto& sendBuffer : m_sendBuffer) {
    sendBuffers.push_back(
        WSABUF{
            .len = static_cast<uint32_t>(sendBuffer->m_size),
            .buf = reinterpret_cast<char*>(sendBuffer->m_buffer)});
  }
  return WSASend(m_socket, sendBuffers.data(), static_cast<DWORD>(sendBuffers.size()), nullptr, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx), nullptr);
}

}  // namespace sh::IO_Engine