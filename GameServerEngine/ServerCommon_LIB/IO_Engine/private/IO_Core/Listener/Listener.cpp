#include "pch.h"
#include <IO_Core/Listener/Listener.h>

namespace sh::IO_Engine {
Listener::Listener(uint16_t port, uint32_t ip)
    : m_iocpHandle(NULL), m_port(port), m_ipAddr(ip), m_listenSocket(NULL) {
}
Listener::Listener(HANDLE iocpHandle, uint16_t port, uint32_t ip)
    : m_iocpHandle(iocpHandle), m_port(port), m_ipAddr(ip), m_listenSocket(NULL) {
}

void Listener::SetAddrInfo(uint16_t port, uint32_t ip) {
  m_port = port;
  m_ipAddr = ip;
}

void Listener::Start(uint16_t inetType, int socketType, int protocolType) {
  m_listenSocket = WSASocket(inetType, socketType, protocolType, NULL, 0, WSA_FLAG_OVERLAPPED);
  SOCKADDR_IN sockAddr{
      .sin_family = inetType,
      .sin_port = htons(m_port),
  };
  sockAddr.sin_addr.s_addr = htonl(m_ipAddr);

  ::bind(m_listenSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(SOCKADDR_IN));
  if (INVALID_SOCKET == m_listenSocket) {
    int getLastError = WSAGetLastError();
    assert(INVALID_SOCKET != m_listenSocket);
  }

  CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_iocpHandle, 1, 0);
}

}  // namespace sh::IO_Engine
