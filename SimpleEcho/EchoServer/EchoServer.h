#pragma once
#include <IO_Engine/IO_Core/IO_Core.h>
#include <IO_Engine/IO_Core/Listener/Listener.h>

namespace sh::EchoServer {
class EchoServer {
 public:
  EchoServer();

  EchoServer(const uint32_t ip, const uint16_t port, const uint16_t ioThreadNo);

  void Start();

 private:
  void AcceptHandle(SOCKET sock);

  IO_Engine::IO_Core m_ioCore;
  IO_Engine::Listener m_listener;
};
}  // namespace sh::EchoServer
