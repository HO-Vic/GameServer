#include "pch.h"
#include "NetworkModule.h"
#include <functional>
#include "../../DreamWorld_Server/Server/MsgProtocol.h"
#include "../Session/Session.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"
#include "../LogManager/LogManager.h"

namespace Stress {
void NetworkModule::InitMsgDispatcher() {
  using namespace std::placeholders;
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::LOGIN_SUCCESS), std::bind(&NetworkModule::OnLoginSuccess, this, _1, _2));
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::INTO_GAME), std::bind(NetworkModule::OnIntoInGame, _1, _2));
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::GAME_STATE_STAGE), std::bind(NetworkModule::OnGameState_Stage, _1, _2));
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::GAME_STATE_BOSS), std::bind(NetworkModule::OnGameState_Boss, _1, _2));
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::GAME_END), std::bind(NetworkModule::OnGameEnd, _1, _2));
  m_msgDispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::STRESS_TEST_DELAY), std::bind(NetworkModule::OnStressTestDelay, _1, _2));
}

void NetworkModule::OnLoginSuccess(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  auto activeCnt = NetworkModule::GetInstance().g_ActiveUserCnt++;
  std::weak_ptr<Session> weakSession = std::static_pointer_cast<Session>(session);
  sessionPtr->InsertJob(
      GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(
          [weakSession]() {
            auto sessionPtr = weakSession.lock();
            if (nullptr == sessionPtr) {
              return;
            }
            sessionPtr->ChangeState(Session::SESSION_STATE::MATCH);
          }));
}

void NetworkModule::OnIntoInGame(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  const DreamWorld::SERVER_PACKET::IntoGamePacket* recvPacket = reinterpret_cast<const DreamWorld::SERVER_PACKET::IntoGamePacket*>(packetHeader);
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  std::weak_ptr<Session> weakSession = std::static_pointer_cast<Session>(session);
  sessionPtr->SetRole(recvPacket->role);
  sessionPtr->InsertJob(
      GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(
          [weakSession]() {
            auto sessionPtr = weakSession.lock();
            if (nullptr == sessionPtr) {
              return;
            }
            sessionPtr->ChangeState(Session::SESSION_STATE::INGAME);
          }));
}

void NetworkModule::OnGameState_Stage(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  const DreamWorld::SERVER_PACKET::GameState_STAGE* recvPacket = reinterpret_cast<const DreamWorld::SERVER_PACKET::GameState_STAGE*>(packetHeader);
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  for (int i = 0; i < 4; ++i) {
    if (recvPacket->userState[i].role == sessionPtr->GetRole()) {
      sessionPtr->SetPosition(recvPacket->userState[i].position.x, recvPacket->userState[i].position.z);
      break;
    }
  }
  // 위치  좌표 W는 여기서하고 렌더링 쓰레드에서 따로 R하지만, 그정도는 감수(위치 움직이는거만 보기 위함)
}

void NetworkModule::OnGameState_Boss(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  const DreamWorld::SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<const DreamWorld::SERVER_PACKET::GameState_BOSS*>(packetHeader);
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  for (int i = 0; i < 4; ++i) {
    if (recvPacket->userState[i].role == sessionPtr->GetRole()) {
      sessionPtr->SetPosition(recvPacket->userState[i].position.x, recvPacket->userState[i].position.z);
      break;
    }
  }
}

void NetworkModule::OnGameEnd(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  std::weak_ptr<Session> weakSession = std::static_pointer_cast<Session>(session);
  sessionPtr->InsertJob(
      GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(
          [weakSession]() {
            auto sessionPtr = weakSession.lock();
            if (nullptr == sessionPtr) {
              return;
            }
            sessionPtr->ChangeState(Session::SESSION_STATE::MATCH);
          }));
}

void NetworkModule::OnStressTestDelay(sh::IO_Engine::TCP_ISessionPtr session, BYTE* packetHeader) {
  auto sessionPtr = std::static_pointer_cast<Session>(session);
  sessionPtr->OnDelayResponse();
}
}  // namespace Stress