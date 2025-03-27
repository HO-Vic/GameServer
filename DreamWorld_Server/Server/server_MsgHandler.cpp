#include "stdafx.h"
#include "Server.h"
#include <string>
#include <memory>
#include <type_traits>
#include <IO_Engine/CommonDefine.h>
#include <IO_Engine/Session/ISession.h>
#include "MsgProtocol.h"
#include "../Network/Session/Session.h"
#include "../DB/DBPlayerLogin.h"
#include "../DB/DBThreadPool.h"

namespace DreamWorld {
void Server::OnLogin(sh::IO_Engine::ISessionPtr session, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::LoginPacket*>(message);
  std::string id = recvPacket->id;
  if (std::string::npos != id.find("module", 0)) {  // 테스트 모듈 테스트 계정은 DB에 없음
    DreamWorld::SERVER_PACKET::LoginPacket loginPacket{};
    session->DoSend(&loginPacket, loginPacket.size);
    std::static_pointer_cast<DreamWorld::Session>(session)->SetName(id.begin(), id.end());
    return;
  }
  // 이거 오브젝트 풀로 관리하면 좋을듯?
  DBJobPtr DBJob = std::make_shared<DBPlayerLogin>(session, recvPacket->id, recvPacket->pw);
  DBThreadPool::GetInstance().InsertDBJob(std::move(DBJob));  // 이러면 DB쓰레드에서 탈거임
}

void Server::OnMatchReq(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::MatchPacket*>(message);
  if (recvPacket->role != ROLE::NONE_SELECT) {
    // Matching::GetInstance().InserMatch(std::static_pointer_cast<UserSession>(shared_from_this()), recvPacket->role);
    // m_playerState = PLAYER_STATE::MATCH;
    // m_matchedRole = recvPacket->role;
  }
}

void Server::OnCancelMatch(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
}

void Server::OnStressDelay(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto sendPacket = DreamWorld::SERVER_PACKET::NotifyPacket(static_cast<char>(DreamWorld::SERVER_PACKET::TYPE::STRESS_TEST_DELAY));
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
}

}  // namespace DreamWorld