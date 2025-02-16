#include <pch.h>
#include "EchoServer.h"
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>
#include <IO_Engine/CommonDefine.h>
#include "Session/SessionManager.h"

namespace sh::EchoServer {
Server::Server()
    : m_ioCore(4), m_listener(), m_acceptor() {
}

Server::Server(const uint32_t ip, const uint16_t port, const uint16_t ioThreadNo)
    : m_ioCore(ioThreadNo), m_listener(port, ip), m_acceptor() {
}

void Server::Init() {
  SessionManager::GetInstance().Init(100);
  m_ioCore.Init();
  m_listener.SetHandle(m_ioCore.GetHandle());
  m_acceptor.Init(m_ioCore.GetHandle(), [&](SOCKET sock) { Server::AcceptHandle(sock); }, 1);
}

void Server::Start() {
  m_ioCore.Start();
  m_listener.Start();
  m_acceptor.SetListenSocket(m_listener.GetListenSocket());

  m_acceptor.Start();

  while (true) {
  }
}

void Server::AcceptHandle(SOCKET sock) {
  // Accept 이후의 행동을 정의
  SessionManager::GetInstance().OnAccept(sock, IO_Engine::IO_TYPE::TCP, RecvHandle, m_ioCore.GetHandle());
}

void Server::RecvHandle(size_t ioByte, BYTE* bufferPosition) {
  // Recv된 데이터 행동 정의
}

}  // namespace sh::EchoServer
