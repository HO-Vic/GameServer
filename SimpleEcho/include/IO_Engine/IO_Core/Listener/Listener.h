#pragma once
#include <WinSock2.h>
#include <ws2def.h>
#include <Windows.h>
#include <cstdint>

namespace sh::IO_Engine {
class Listener {
 public:
  Listener(HANDLE iocpHandle, uint16_t port = 9000, uint32_t ip = ADDR_ANY);

  void SetAddrInfo(uint16_t port = 9000, uint32_t ip = ADDR_ANY);

  void Start(uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP);

  SOCKET GetListenSocket() const {
    return m_listenSocket;
  }

 private:
  SOCKET m_listenSocket;
  HANDLE m_iocpHandle;
  uint32_t m_ipAddr;
  uint16_t m_port;
};
}  // namespace sh::IO_Engine
