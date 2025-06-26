#pragma once
#include <list>
#include <ws2def.h>
#include <mutex>
#include <vector>
#include <memory>
#include "Utility/Thread/IWorkerItem.h"
#include "Buffer/SendBuffer.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
// UDP는 1:n 통신이 가능하기 때문에, Session당 소켓 x
// UDP_IAgent의 소켓을 사용하여 Send
class UDP_ISession {
 public:
  UDP_ISession() = default;

  UDP_ISession(const SOCKADDR& toAddr)
      : m_toAddr(toAddr) {
  }

  virtual ~UDP_ISession() = default;

  uint32_t DoSend(SOCKET sock, const BYTE* data, uint32_t len) const;

 private:
  SOCKADDR m_toAddr;
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

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

 private:
  SOCKADDR m_toAddr;
  std::list<InternalBatchSendBuffer> m_buffers;
  std::mutex m_bufferLock;
};
}  // namespace sh::IO_Engine
