#include "stdafx.h"
#include "Server.h"
#include <functional>
#include <string>
#include <spdlog/common.h>
#include <IO_Engine/Session/ISession.h>
#include "../Network/Session/SessionMananger.h"
#include "../Network/Session/Session.h"
#include "../MsgDispatcher/MsgDispatcher.h"
#include "../Room/RoomMsgDispatcher.h"
#include "MsgProtocol.h"
#include "../LogManager/LogManager.h"

namespace DreamWorld {
using logLevel = spdlog::level::level_enum;
Server::Server(uint16_t ioThreadNo)
    : m_ioCore(ioThreadNo) {
}

void Server::Init() {
  m_ioCore.Init();
  m_listener.Init(m_ioCore.GetHandle());
  m_acceptor.Init(m_ioCore.GetHandle(), [&](SOCKET sock) {
    AcceptHandle(sock);
  });

  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::LOGIN), std::bind(Server::OnLogin, std::placeholders::_1, std::placeholders::_2));
  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MATCH), std::bind(Server::OnStartMatch, std::placeholders::_1, std::placeholders::_2));
  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MATCH_REQUEST), std::bind(Server::OnCancelMatch, std::placeholders::_1, std::placeholders::_2));
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
  SessionMananger::GetInstance().OnAccept(sock, sh::IO_Engine::TCP, std::bind(&Server::RecvHandle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), m_ioCore.GetHandle());
}

void Server::RecvHandle(sh::IO_Engine::ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition) {
  auto packetHeader = reinterpret_cast<PacketHeader*>(bufferPosition);
  MsgHandler handler = nullptr;
  if (!m_dispatcher.GetHandler(packetHeader->type, handler)) {
    // 플레이어가 인게임(룸)인지 확인하고 디스패치
    // auto userPtr = std::static_pointer_cast<DreamWorld::Sesssion>(sessionPtr);
    if (!RoomMsgDispatcher::GetInstance().GetHandler(packetHeader->type, handler)) {
      WRITE_LOG(logLevel::err, "{}({}) > Can not Find Room Msg!", __FUNCTION__, __LINE__);
      return;
    }
  }
  handler(sessionPtr, bufferPosition);
}
void Server::OnLogin(sh::IO_Engine::ISessionPtr session, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::LoginPacket*>(message);
  std::string id = recvPacket->id;
  if (std::string::npos != id.find("module", 0)) {
    DreamWorld::SERVER_PACKET::LoginPacket loginPacket{};
    session->DoSend(&loginPacket, loginPacket.size);
    std::static_pointer_cast<DreamWorld::Session>(session)->SetName(id.begin(), id.end());
    return;
  }
  // std::shared_ptr<DB::EventBase> getPlayerInfoEvent = std::make_shared<DB::PlayerInfoEvent>(DB::DB_OP_CODE::DB_OP_GET_PLAYER_INFO, std::static_pointer_cast<UserSession>(shared_from_this()), recvPacket->id, recvPacket->pw);
  // DB::DBConnector::GetInstance().InsertDBEvent(getPlayerInfoEvent);
}
void Server::OnStartMatch(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
}
void Server::OnCancelMatch(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
}
}  // namespace DreamWorld