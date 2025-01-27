#pragma once
#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/RecvContext/RecvContext.h>

// Overlapped가 필요함
namespace sh::IO_Engine {

class SessionImpl {
 public:
  SessionImpl(SOCKET sock, const IO_TYPE ioType, RecvHandler&& recvHandler)
      : m_sendContext(sock, ioType), m_recvContext(ioType, sock, recvHandler) {
  }

  void SendComplete(const size_t ioByte) {
    m_sendContext.SendComplete(ioByte);
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

  void DoRecv() {
    int32_t recvError = m_recvContext.DoRecv();
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