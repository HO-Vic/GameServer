#pragma once
#include <list>
#include <WinSock2.h>
#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include "Utility/Thread/IWorkerItem.h"
// #include "Buffer/SendBuffer.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
// UDP는 1:n 통신이 가능하기 때문에, Session당 소켓 x
// UDP_IAgent의 소켓을 사용하여 Send
// 하트비트를 둬서 관리 추가 해야할듯?
class UDP_IAgent;
class SendBuffer;
class UDP_ISession
    : public std::enable_shared_from_this<UDP_ISession> {
 public:
  UDP_ISession() = default;

  UDP_ISession(const sockaddr_in& toAddr)
      : m_toAddr(toAddr), m_isAlive(true) {
  }

  // virtual ~UDP_ISession() = default;

  virtual void OnUDPSessionTimeout() = 0;  // 하트비트, 재전송 횟수 초과 시, 종료할 때 호출

  uint32_t DoSend(std::shared_ptr<UDP_IAgent>& agentPtr, const BYTE* data, uint32_t len);

  void Close() {
    m_isAlive = false;
  }

  bool IsAlive() const {
    return m_isAlive;
  }

  const sockaddr_in& GetAddrInfo() const {
    return m_toAddr;
  }

 private:
  sockaddr_in m_toAddr;
  std::atomic_bool m_isAlive = true;
};

class [[deprecated("Exec에서 socket을 알 수 없어서 완료 후, 일괄 전송 안되는데, 나중에 다시 고민")]] UDP_IBatchSession
    : public Utility::IWorkerItem {
  struct InternalBatchSendBuffer {
    std::vector<std::shared_ptr<SendBuffer>> m_buffers;
    uint32_t m_totalLen = 0;
    constexpr bool CanInsert(const uint32_t insertSize) const {
      return MTUSize - m_totalLen >= insertSize;
    }
    static constexpr uint32_t MTUSize = 1200;

    InternalBatchSendBuffer() = default;
  };

 public:
  UDP_IBatchSession() = default;

  UDP_IBatchSession(const SOCKADDR& toAddr)
      : m_toAddr(toAddr) {
  }

  void DoSend(SOCKET sock, const BYTE* data, uint32_t len);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

 private:
  SOCKADDR m_toAddr;
  std::list<InternalBatchSendBuffer> m_buffers;
  std::mutex m_bufferLock;
};
}  // namespace sh::IO_Engine
