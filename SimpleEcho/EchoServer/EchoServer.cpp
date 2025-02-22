#include <pch.h>
#include "EchoServer.h"
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>
#include <IO_Engine/CommonDefine.h>
#include "Session/SessionManager.h"
#include "Packet.h"
#include "../LogManager/LogManager.h"
#include <spdlog/common.h>

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
  WRITE_LOG(spdlog::level::info, "{}({}) > init success", __FUNCTION__, __LINE__);
}

void Server::Start() {
  m_ioCore.Start();
  m_listener.Start();

  m_acceptor.SetListenSocket(m_listener.GetListenSocket());
  m_acceptor.Start();
  WRITE_LOG(spdlog::level::info, "{}({}) > Server Start", __FUNCTION__, __LINE__);
  while (true) {
  }
}

void Server::AcceptHandle(SOCKET sock) {
  // Accept 이후의 행동을 정의
  WRITE_LOG(spdlog::level::info, "{}({}) > On Accept Player", __FUNCTION__, __LINE__);
  SessionManager::GetInstance().OnAccept(sock, IO_Engine::IO_TYPE::TCP, RecvHandle, m_ioCore.GetHandle());
}

void Server::RecvHandle(IO_Engine::ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition) {
  WRITE_LOG(spdlog::level::info, "{}({}) > Recv Packet", __FUNCTION__, __LINE__);
  PacketHeader* packet = reinterpret_cast<PacketHeader*>(bufferPosition);
  //  Recv된 데이터 행동 정의
  switch (packet->type) {
    case PACKET_TYPE::SIMPLE_MSG: {
      SimpleMsgPacket* decodedPacket = reinterpret_cast<SimpleMsgPacket*>(bufferPosition);
      WRITE_LOG(spdlog::level::info, "{}({}) > Send Packet", __FUNCTION__, __LINE__);
      sessionPtr->DoSend(reinterpret_cast<BYTE*>(decodedPacket), ioByte);
    } break;
    default:
      break;
  }
}

}  // namespace sh::EchoServer
