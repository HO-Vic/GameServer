#include <pch.h>
#include <Session/SendContext/TCP_SendContextImpl.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
void TCP_SendContextImpl::DoSend(const BYTE* data, const size_t len) {
  static constexpr bool SEND_DESIRE = false;
  auto sendData = SendBufferPool::GetInstance().MakeShared(data, len);
  {
    std::lock_guard<std::mutex> lg(m_queueLock);
    m_sendQueue.push(sendData);
  }
  if (m_isSendAble) {
    bool expectedValue = true;
    bool isSendAbleThread = m_isSendAble.compare_exchange_strong(expectedValue, SEND_DESIRE);
    if (isSendAbleThread) {
      SendExecute();
    }
  }
}

void TCP_SendContextImpl::SendComplete(const size_t ioByte) {
  // 여기서는 보낼게 없을 때만, sendAble을 변경하고
  // 보낼게 있다면 그대로 상태 유지
  {
    std::lock_guard<std::mutex> lg(m_queueLock);
    if (0 == m_sendQueue.size()) {
      m_isSendAble = true;
      return;
    }
  }
  SendExecute();
}

void TCP_SendContextImpl::SendExecute() {
  m_sendBuffer.clear();
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
  WSASend(m_socket, sendBuffers.data(), sendBuffers.size(), nullptr, 0, nullptr, nullptr);
}

}  // namespace sh::IO_Engine