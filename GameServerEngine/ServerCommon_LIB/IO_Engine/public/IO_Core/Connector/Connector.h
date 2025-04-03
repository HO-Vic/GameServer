#pragma once
#include <WinSock2.h>
#include <Utility/Thread/IWorkerItem.h>

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
class Connector {
 public:
  Connector(HANDLE ioHandle, uint32_t ipAddr, uint16_t port);

  // success, errorCode
  std::pair<bool, int> TryConnect();

 private:
  HANDLE m_ioHandle;
};

class ConnectEvent
    : public Utility::IWorkerItem {
 public:
  ConnectEvent(SOCKET sock);

  virtual void Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const ULONG errorCode) override;

  std::function<void()> GetTimeOutFunc();

  virtual void OnConnect(std::shared_ptr<ISession>&& sessionPtr) = 0;  // 커넥트 이후에 해야할 행동 정의

  virtual void FailConnect() = 0;

 private:
  SOCKET m_socket;
};
}  // namespace sh::IO_Engine
