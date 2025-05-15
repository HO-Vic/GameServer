#pragma once
#include <minwindef.h>
#include <cstdint>
#include "Utility/Thread/IWorkerItem.h"
#include "Utility/Pool/ObjectPool.h"
#include "Utility/SingletonBase/Singleton.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
constexpr static uint32_t MAX_SEND_BUFFER_SIZE = 1024;
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

  UDP_SingleSendBuffer(const BYTE* data, const uint32_t len);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

  void DoSend(SOCKET sock, const SOCKADDR& toAddr);

 private:
  WSABUF m_wsaBuf;
  // void SendComplete();
};

}  // namespace sh::IO_Engine