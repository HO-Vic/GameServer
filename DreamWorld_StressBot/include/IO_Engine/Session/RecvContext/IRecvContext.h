#pragma once
#include <WinSock2.h>
#include "../../CommonDefine.h"

namespace sh {
namespace Utility {
class ThWorkerJob;
}
}  // namespace sh::Utility

namespace sh::IO_Engine {
class IRecvContext {
 public:
  IRecvContext(SOCKET sock, RecvHandler&& recvHandler)
      : m_socket(sock), m_buffer(""), m_remainLen(0), m_recvHandler(recvHandler) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = static_cast<uint32_t>(m_remainLen);
  }

  virtual int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize) = 0;

  virtual int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob) = 0;

 protected:
  SOCKET m_socket;
  RecvHandler m_recvHandler;
  WSABUF m_wsaBuf;
  BYTE m_buffer[MAX_RECV_BUF_SIZE];
  size_t m_remainLen;
};
}  // namespace sh::IO_Engine
