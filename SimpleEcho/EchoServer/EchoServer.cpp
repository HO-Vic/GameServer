#include <pch.h>
#include "EchoServer.h"
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>

namespace sh::EchoServer {
EchoServer::EchoServer()
    : m_ioCore(4), m_listener() {
  m_ioCore.Init();
}

EchoServer::EchoServer(const uint32_t ip, const uint16_t port, const uint16_t ioThreadNo)
    : m_ioCore(ioThreadNo), m_listener(port, ip) {
}

void EchoServer::Start() {
  m_ioCore.Start();
  m_listener.Start();

  // Acceptor도 Init으로 초기화 하는게 날거같음, 물론 이 코드에서는 문제x
  sh::IO_Engine::Acceptor acceptor(m_ioCore.GetHandle(), m_listener.GetListenSocket(), [&](SOCKET sock) { EchoServer::AcceptHandle(sock); }, 1);
  acceptor.Start();

  while (true) {
  }
}

void EchoServer::AcceptHandle(SOCKET sock) {
  // Accept 이후의 행동을 정의
}

}  // namespace sh::EchoServer
