#pragma once
#include <WinSock2.h>
#include "../IO_Core/OverlappedEvent/IOverlappedEvent.h"
#include "../CommonDefine.h"

namespace sh::IO_Engine {
class SessionImpl;
class ISession
    : public IOverlappedEvent {
 public:
  ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle);

  virtual ~ISession();

  void DoSend(const BYTE* data, const size_t len);

  virtual void Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) override;

 protected:
  void StartRecv();

  // 위에 레이어에서 상속받아서 Disconnect 상황에서 해야하는 일 정의
  virtual void Disconnect() = 0;

 private:
  SessionImpl* m_sessionImpl;
};
}  // namespace sh::IO_Engine
