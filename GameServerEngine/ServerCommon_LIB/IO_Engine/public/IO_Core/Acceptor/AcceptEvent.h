#pragma once
#include <pch.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
class OverlappedEx;
class AcceptEvent
    : public IOverlappedEvent {
 public:
  AcceptEvent(SOCKET listenSocket, uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP)
      : m_listenSocket(listenSocket), m_clientSocket(NULL), m_inetType(inetType), m_socketType(socketType), m_protocolType(protocolType) {
    ZeroMemory(&m_connInfo, sizeof(ConnectInfo));
  }

  void Start(OverlappedEx* overlappedEx);

  virtual void Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) override;

 private:
  SOCKET m_clientSocket;
  SOCKET m_listenSocket;
  ConnectInfo m_connInfo;
  uint16_t m_inetType;
  int m_socketType;
  int m_protocolType;
};
}  // namespace sh::IO_Engine
