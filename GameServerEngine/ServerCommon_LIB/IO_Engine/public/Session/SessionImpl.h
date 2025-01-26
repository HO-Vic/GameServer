#pragma once
#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/RecvContext/RecvContext.h>

// Overlapped가 필요함
namespace sh::IO_Engine {

class SessionImpl {
 public:
  SessionImpl(SOCKET sock, const IO_TYPE sendType, RecvHandler recvHandler)
      : m_sendContext(sock, sendType), m_recvContext(sock, recvHandler) {
  }

  void DoSend(const BYTE* data, const size_t len) {
    m_sendContext.DoSend(data, len);
  }

  void RecvCompletion(size_t ioByte) {
    if (0 == ioByte) {
      // Disconnent
    }
    int32_t recvError = m_recvContext.RecvCompletion(ioByte);
    if (0 != recvError) {
      // Error
      // Disconnect
    }
  }

 private:
  SendContext m_sendContext;
  RecvContext m_recvContext;
};
}  // namespace sh::IO_Engine