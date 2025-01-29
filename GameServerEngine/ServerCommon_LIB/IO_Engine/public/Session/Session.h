#pragma once
#include <pch.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
class SessionImpl;
class Session
    : public IOverlappedEvent {
 public:
  Session(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle);

  virtual ~Session();

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
