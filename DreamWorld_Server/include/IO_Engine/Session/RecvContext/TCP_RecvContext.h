#pragma once
#include <cstdint>

namespace sh ::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class TCP_RecvContext {
 public:
  TCP_RecvContext() = default;

  TCP_RecvContext(SOCKET sock, TCP_RecvHandler&& TCP_RecvHandler)
      : m_socket(sock), m_buffer(""), m_remainLen(0), m_recvHandler(TCP_RecvHandler) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = m_remainLen;
  }

  int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, DWORD ioSize);

  int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob);

 private:
  SOCKET m_socket;
  TCP_RecvHandler m_recvHandler;
  WSABUF m_wsaBuf;
  BYTE m_buffer[MAX_RECV_BUF_SIZE];
  uint32_t m_remainLen;
};
}  // namespace sh::IO_Engine
