#pragma once
#include <cstdint>
#include <IO_Engine/IO_Core/IO_Core.h>
#include <IO_Engine/IO_Core/Listener/Listener.h>
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>

namespace sh::EchoServer {
class Server {
 public:
  Server();

  Server(const uint32_t ip, const uint16_t port, const uint16_t ioThreadNo);

  void Start();

  void Init();

 private:
  void AcceptHandle(SOCKET sock);
  static void RecvHandle(size_t ioByte, BYTE* bufferPosition);

  IO_Engine::IO_Core m_ioCore;
  IO_Engine::Listener m_listener;
  IO_Engine::Acceptor m_acceptor;
};
}  // namespace sh::EchoServer
