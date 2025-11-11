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

  UDP_RecvContext(UDP_IAgentPtr& agentPtr, UDP_RecvHandler UDP_RecvHandler)
      : m_agentPtr(agentPtr), m_buffer(""), m_recvHandler(std::move(UDP_RecvHandler)) {
    m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBuf.len = MAX_RECV_BUF_SIZE;
  }

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode);

  void RecvComplete(uint32_t ioSize, std::shared_ptr<UDP_IAgent>& agentPtr);

  int32_t DoRecv(Utility::ThWorkerJob* workerJob, std::shared_ptr<UDP_IAgent>& agentPtr);

 private:
  std::weak_ptr<UDP_IAgent> m_agentPtr;
  UDP_RecvHandler m_recvHandler = nullptr;
  WSABUF m_wsaBuf{};
  BYTE m_buffer[MAX_RECV_BUF_SIZE] = {0};
  sockaddr_in m_recvAddr{};
  int32_t m_recvAddrSize = sizeof(sockaddr_in);
};
}  // namespace sh::IO_Engine
