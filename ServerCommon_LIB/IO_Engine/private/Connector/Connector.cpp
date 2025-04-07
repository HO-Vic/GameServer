#include "pch.h"
#include <IO_Core/Connector/Connector.h>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
ConnectorBase::ConnectorBase(const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType)
    : m_inetType(inetType), m_socketType(socketType), m_protocolType(protocolType) {
  ZeroMemory(&m_connectAddr, sizeof(SOCKADDR_IN));
  m_connectAddr.sin_family = m_inetType;
  m_connectAddr.sin_port = htons(port);
  inet_pton(m_inetType, ipAddr.data(), &m_connectAddr.sin_addr.s_addr);
}

SyncConnector::SyncConnector(const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType)
    : ConnectorBase(ipAddr, port, inetType, socketType, protocolType) {
}

bool SyncConnector::TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) {
  SOCKET connSocket = WSASocketW(m_inetType, m_socketType, m_protocolType, NULL, NULL, WSA_FLAG_OVERLAPPED);
  if (NULL == connSocket) {
    if (connSocket == INVALID_SOCKET) {
      failHandle(WSAGetLastError());
      return false;
    }
  }
  // else {
  //   // Retry 소켓인데, valid 체크해야 됨
  // }

  static constexpr size_t infoSize = sizeof(SOCKADDR_IN);
  DWORD sentByte = 0;
  bool result = connect(connSocket, reinterpret_cast<sockaddr*>(&m_connectAddr), static_cast<int>(infoSize));
  if (result) {
    successHandle(connSocket);
    connSocket = NULL;
    return true;
  }

  failHandle(WSAGetLastError());
  closesocket(connSocket);
  connSocket = NULL;
  return false;
}
}  // namespace sh::IO_Engine