#pragma once
#include <WinSock2.h>
#include <Utility/Thread/IWorkerItem.h>

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
class ConnectEvent
    : public Utility::IWorkerItem {
 public:
  virtual void Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte) override;

  void TryConnect();

  std::function<void()> TryTimeOut();

  virtual void OnConnect(std::shared_ptr<ISession>&& sessionPtr) = 0;  // 커넥트 이후에 해야할 행동 정의

 private:
  SOCKET m_socket;
};
}  // namespace sh::IO_Engine
