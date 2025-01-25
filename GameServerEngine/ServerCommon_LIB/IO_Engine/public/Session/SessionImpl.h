#pragma once
#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/RecvContext.h>

// Overlapped가 필요함
namespace sh::IO_Engine {
class SessionImpl {
 public:
  SessionImpl(SOCKET sock, const SEND_TYPE sendType)
      : m_sendContext(sock, sendType), m_recvContext(sock) {
  }

  void DoSend(const BYTE* data, const size_t len) {
    m_sendContext.DoSend(data, len);
  }

  void DoRecv() {
    // m_recvContext.
  }

 private:
  SendContext m_sendContext;
  RecvContext m_recvContext;
};
}  // namespace sh::IO_Engine