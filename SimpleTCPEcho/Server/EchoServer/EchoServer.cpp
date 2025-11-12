#include "pch.h"
#include "EchoServer.h"
#include <IO_Engine/IO_Core/Acceptor/Acceptor.h>
#include <IO_Engine/CommonDefine.h>
#include <IO_Engine/Buffer/SendBuffer.h>
#include <IO_Engine/Buffer/SendBufferPool.h>
#include "Session/SessionManager.h"
#include "Packet.h"
#include "../LogManager/LogManager.h"

namespace SimpleTCP {
Server::Server()
    : m_ioCore(4), m_listener(), m_acceptor() {
}

Server::Server(const uint32_t ip, const uint16_t port, const uint8_t ioThreadNo)
    : m_ioCore(ioThreadNo), m_listener(port, ip), m_acceptor() {
}

void Server::Init() {
  InitMsg();
  SessionManager::GetInstance().Init(100);
  m_ioCore.Init();
  m_listener.SetHandle(m_ioCore.GetHandle());
  m_acceptor.Init(m_ioCore.GetHandle(), [&](SOCKET sock) { Server::AcceptHandle(sock); }, 1);
  WRITE_LOG(spdlog::level::info, "{}({}) > init success", __FUNCTION__, __LINE__);
}

void Server::OnSimpleMsg(sh::IO_Engine::TCP_SessionBasePtr sessionPtr, BYTE* packetHeader) {
  auto msgPacket = reinterpret_cast<SimpleMsgPacket*>(packetHeader);
  SimpleMsg simpleMsg{};
  simpleMsg.Deserialize(msgPacket);
  auto packetSize = simpleMsg.GetSerializeSize();
  auto sendBuffer = sh::IO_Engine::SendBufferAllocator::GetInstance().GetShared(packetSize);
  auto writePtr = sendBuffer->GetWritePtr(packetSize);
  simpleMsg.Serialize(writePtr, packetSize);
  WRITE_LOG(LogLevel::info, "RecvComplete, [PayloadSize: {}] MSG: {}", simpleMsg.GetPayloadMsg().size(), simpleMsg.GetPayloadMsg().c_str());
  sessionPtr->DoSend(std::move(sendBuffer));
}

void Server::OnParsedMsg(sh::IO_Engine::TCP_SessionBasePtr sessionPtr, BYTE* packetHeader) {
  auto msgPacket = reinterpret_cast<ParseMsgPacket*>(packetHeader);
  ParsedMsg parsedMsg{};
  parsedMsg.Deserialize(msgPacket);
  auto& payload = parsedMsg.GetPayload();
  uint32_t msgSize = 0;
  for (const auto& data : payload) {
    WRITE_LOG(LogLevel::info, "RecvComplete Parsed MSG: {}", data);
    msgSize += static_cast<uint32_t>(data.size());
  }
  WRITE_LOG(LogLevel::info, "RecvComplete [Payload:{}]", msgSize);

  auto packetSize = parsedMsg.GetSerializeSize();
  auto sendBuffer = sh::IO_Engine::SendBufferAllocator::GetInstance().GetShared(packetSize);
  auto writePtr = sendBuffer->GetWritePtr(packetSize);
  parsedMsg.Serialize(writePtr, packetSize);
  sessionPtr->DoSend(std::move(sendBuffer));
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
  using namespace std::placeholders;
  WRITE_LOG(spdlog::level::info, "{}({}) > On Accept Player", __FUNCTION__, __LINE__);
  SessionManager::GetInstance().OnAccept(sock, sh::IO_Engine::IO_TYPE::TCP, std::bind(&Server::RecvHandle, this, _1, _2, _3), m_ioCore.GetHandle());
}

void Server::RecvHandle(sh::IO_Engine::TCP_SessionBasePtr sessionPtr, size_t ioByte, BYTE* bufferPosition) {
  MsgHandle msgHandle = nullptr;
  auto packetHeader = reinterpret_cast<PacketHeader*>(bufferPosition);
  if (!m_dispatcher.GetHandle(packetHeader->type, msgHandle)) {
    WRITE_LOG(spdlog::level::info, "{}({}) > Unknown packet", __FUNCTION__, __LINE__);
    return;
  }
  msgHandle(sessionPtr, bufferPosition);
}

}  // namespace SimpleTCP
