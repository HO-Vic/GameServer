#pragma once
#include <cstdint>
#include <IO_Engine/IO_Core/IO_Core.h>
#include <IO_Engine/IO_Core/Listener/Listener.h>
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>
#include <IO_Engine/CommonDefine.h>

namespace sh::EchoServer {
class TCP_ISession;
class Server {
 public:
  Server();

  Server(const uint32_t ip, const uint16_t port, const uint8_t ioThreadNo);

  void Start();

  void Init();

 private:
  void AcceptHandle(SOCKET sock);
  static void RecvHandle(IO_Engine::TCP_ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition);

  IO_Engine::IO_Core m_ioCore;
  IO_Engine::Listener m_listener;
  IO_Engine::Acceptor m_acceptor;
};
}  // namespace sh::EchoServer
