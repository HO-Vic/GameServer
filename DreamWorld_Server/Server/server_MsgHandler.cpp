#include "stdafx.h"
#include "Server.h"
#include <string>
#include <memory>
#include <type_traits>
#include <IO_Engine/CommonDefine.h>
#include <IO_Engine/Session/TCP_ISession.h>
#include "MsgProtocol.h"
#include "../Network/Session/Session.h"
#include "../DB/DBPlayerLogin.h"
#include "../DB/DBThreadPool.h"
#include "../Match/Matching.h"
#include "../ObjectPools.h"
#include "../LogManager/LogManager.h"

namespace DreamWorld {
void Server::OnLogin(sh::IO_Engine::TCP_ISessionPtr session, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::LoginPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::LoginPacket*>(message);
  std::string id = recvPacket->id;
  if (std::string::npos != id.find("module", 0)) {  // 테스트 모듈 테스트 계정은 DB에 없음
    DreamWorld::SERVER_PACKET::LoginPacket loginPacket{};
    session->DoSend(&loginPacket, loginPacket.size);
    std::static_pointer_cast<DreamWorld::Session>(session)->SetName(id.begin(), id.end());
    return;
  }
  // 이거 오브젝트 풀로 관리하면 좋을듯?
  DBJobPtr dbJob = ObjectPool<DBPlayerLogin>::GetInstance().MakeShared(session, recvPacket->id, recvPacket->pw);
  DBThreadPool::GetInstance().InsertDBJob(std::move(dbJob));  // 이러면 DB쓰레드에서 탈거임
}

void Server::OnMatchReq(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::MatchPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::MatchPacket*>(message);
  if (recvPacket->role == ROLE::NONE_SELECT) {
    WRITE_LOG(logLevel::err, "{}({}) > Invalid Role", __FUNCTION__, __LINE__);
  }
  auto userSession = std::static_pointer_cast<Session>(sessionPtr);
  Matching::GetInstance().InsertMatch(userSession, recvPacket->role);
  userSession->SetIngameRole(recvPacket->role);
  // m_playerState = PLAYER_STATE::MATCH;
  // m_matchedRole = recvPacket->role;
}

void Server::OnStressDelay(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message) {
  auto sendPacket = DreamWorld::SERVER_PACKET::NotifyPacket(static_cast<char>(DreamWorld::SERVER_PACKET::TYPE::STRESS_TEST_DELAY));
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
}

}  // namespace DreamWorld