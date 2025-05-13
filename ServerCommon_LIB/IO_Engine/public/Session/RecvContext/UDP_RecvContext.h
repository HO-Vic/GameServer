#pragma once
#include <cstdint>
// #include <Session/RecvContext/IRecvContext.h>

namespace sh ::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class UDP_RecvContext {
 public:
  UDP_RecvContext(SOCKET sock, RecvHandler&& recvHandler)
      : m_buffer(""), m_recvHandler(recvHandler) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = MAX_RECV_BUF_SIZE;
  }
  int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize);

  int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob);

 private:
  RecvHandler m_recvHandler;
  WSABUF m_wsaBuf;
  BYTE m_buffer[MAX_RECV_BUF_SIZE];
};
}  // namespace sh::IO_Engine
