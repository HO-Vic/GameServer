#pragma once
#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/RecvContext/RecvContext.h>

// Overlapped가 필요함
namespace sh::IO_Engine {
class OverlappedEx;
class SessionImpl {
 public:
  SessionImpl(SOCKET sock, const IO_TYPE ioType, RecvHandler&& recvHandler)
      : m_sendContext(sock, ioType), m_recvContext(ioType, sock, recvHandler) {
  }

  void SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) {
    m_sendContext.SendComplete(overlappedEx, ioByte);
  }

  void DoSend(const BYTE* data, const size_t len) {
    m_sendContext.DoSend(data, len);
  }

  void RecvComplete(OverlappedEx* overlappedEx, size_t ioByte) {
    if (0 == ioByte) {
      // Disconnent
    }
    int32_t recvError = m_recvContext.RecvComplete(overlappedEx, ioByte);
    if (0 != recvError) {
      // Error
      // Disconnect
    }
  }

  void StartRecv() {
    int32_t recvError = m_recvContext.StartRecv();
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