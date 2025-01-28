#pragma once
#include <pch.h>

namespace sh::IO_Engine {
class OverlappedEx;
class IOverlappedEvent;
using OverlappedPtr = std::shared_ptr<IOverlappedEvent>;
class ISendContextImpl {
 public:
  ISendContextImpl(SOCKET sock)
      : m_socket(sock) {
  }

  virtual void DoSend(OverlappedPtr& session, const BYTE* data, const size_t len) = 0;

  virtual void SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) = 0;

 protected:
  virtual void SendExecute(OverlappedEx* overlappedEx) = 0;

 protected:
  SOCKET m_socket;
};
}  // namespace sh::IO_Engine
