#pragma once
#include <pch.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
class SessionImpl;
class Session
    : public IOverlappedEvent {
 public:
  Session(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler);

  virtual ~Session();

  void DoSend(const BYTE* data, const size_t len);

  void DoRecv();

  virtual void Execute(const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) override;

 private:
  void SendComplete(const size_t ioByte);

  void RecvComplete(const size_t ioByte);

 private:
  SessionImpl* m_sessionImpl;
};
}  // namespace sh::IO_Engine
