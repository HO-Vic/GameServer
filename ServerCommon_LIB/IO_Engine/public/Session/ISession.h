#pragma once
#include <atomic>
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"

namespace sh::IO_Engine {
class SessionImpl;
class ISession
    : public Utility::IWorkerItem {
 public:
  ISession();

  ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle);

  virtual ~ISession();

  void DoSend(const void* data, const size_t len);

  virtual void Execute(Utility::ThWorkerJob* thWorkerItem, const DWORD ioByte, const uint64_t errorCode) override;

  void StartRecv();

 protected:
  // 위에 레이어에서 상속받아서 Disconnect 상황에서 해야하는 일 정의
  virtual void OnDisconnect() = 0;

 private:
  void Disconnect();

 private:
  SessionImpl* m_sessionImpl;
  std::atomic_bool m_isDisconnnected;
};
}  // namespace sh::IO_Engine
