#pragma once
#include <cstdint>

namespace sh ::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class UDP_ISession;
class UDP_RecvContext {
 public:
  UDP_RecvContext() = default;

  UDP_RecvContext(RecvHandler&& recvHandler)
      : m_buffer(""), m_recvHandler(recvHandler) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = MAX_RECV_BUF_SIZE;
  }

  int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize, SOCKET sock);

  int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob, SOCKET sock);

 private:
  RecvHandler m_recvHandler = nullptr;
  WSABUF m_wsaBuf;
  BYTE m_buffer[MAX_RECV_BUF_SIZE];

  sockaddr addr;
  std::weak_ptr<UDP_ISession> m_sessionPtr;
};
}  // namespace sh::IO_Engine
