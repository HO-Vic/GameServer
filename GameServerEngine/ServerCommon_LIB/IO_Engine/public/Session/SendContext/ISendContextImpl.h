#pragma once
#include <pch.h>

namespace sh::IO_Engine {
class ISendContextImpl {
 public:
  ISendContextImpl(SOCKET sock)
      : m_socket(sock) {
  }

  virtual void DoSend(const BYTE* data, const size_t len) = 0;

  virtual void SendComplete(const size_t ioByte) = 0;

 protected:
  virtual void SendExecute() = 0;

 protected:
  SOCKET m_socket;
};
}  // namespace sh::IO_Engine
