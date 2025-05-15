#pragma once
#include <cstdint>
#include "Utility/Thread/IWorkerItem.h"
namespace sh ::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class UDP_IAgent;
class UDP_RecvContext
    : public Utility::IWorkerItem {
 public:
  UDP_RecvContext() = default;

  UDP_RecvContext(UDP_RecvHandler&& TCP_RecvHandler)
      : m_buffer(""), m_recvHandler(TCP_RecvHandler) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = MAX_RECV_BUF_SIZE;
  }

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode);

  void RecvComplete(uint32_t ioSize, std::shared_ptr<UDP_IAgent>& sessionPtr);

  int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob, std::shared_ptr<UDP_IAgent>& sessionPtr);

 private:
  std::weak_ptr<UDP_IAgent> m_sessionPtr;
  UDP_RecvHandler m_recvHandler = nullptr;
  WSABUF m_wsaBuf;
  BYTE m_buffer[MAX_RECV_BUF_SIZE];
};
}  // namespace sh::IO_Engine
