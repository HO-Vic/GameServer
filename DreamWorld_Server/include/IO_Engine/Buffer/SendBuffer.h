#pragma once
#include <cstdint>
#include <memory>
#include "Utility/Thread/IWorkerItem.h"
#include "Utility/Pool/ObjectPool.h"
#include "Utility/SingletonBase/Singleton.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
constexpr static uint32_t MAX_SEND_BUFFER_SIZE = 1024;
class UDP_IAgent;
class UDP_ISession;

class SendBuffer {
 public:
  SendBuffer() = default;
  SendBuffer(const BYTE* data, const uint32_t len);

 public:
  uint32_t m_size;
  BYTE m_buffer[MAX_SEND_BUFFER_SIZE];
};

class UDP_SingleSendBuffer final
    : public Utility::IWorkerItem,
      public SendBuffer {
 public:
  UDP_SingleSendBuffer() = default;

  UDP_SingleSendBuffer(std::shared_ptr<UDP_IAgent>& agentPtr, std::shared_ptr<UDP_ISession>& sessionPtr, const BYTE* data, const uint32_t len);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

  uint32_t DoSend(SOCKET sock, sockaddr_in toAddr);

 private:
  WSABUF m_wsaBuf;
  std::weak_ptr<UDP_IAgent> m_agentPtr;
  std::weak_ptr<UDP_ISession> m_sessionPtr;
  uint32_t m_retransmitCnt = 0;
};

}  // namespace sh::IO_Engine