#include "pch.h"
#include <random>
#include "StressFSM.h"
#include "../Session/Session.h"
#include "../LogManager/LogManager.h"
#include "../StressInGame/Character/CharacterObject.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"

namespace Stress {
void LoginState::EnterState() {
  auto sessionPtr = m_session.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  DreamWorld::CLIENT_PACKET::LoginPacket sendPacket{};
  sprintf_s(sendPacket.id, "module%ld", sessionPtr->GetUniqueNo());
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
}

void LoginState::UpdateState() {
}

void LoginState::ExitState() {
}

void MatchState::EnterState() {
  auto sessionPtr = m_session.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::uniform_int_distribution randRole(0, 3);
  auto r = static_cast<ROLE>(std::pow(2, randRole(dre)));
  DreamWorld::CLIENT_PACKET::MatchPacket sendPacket{r};
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
}

void MatchState::UpdateState() {
}

void MatchState::ExitState() {
}

void InGameState::EnterState() {
  auto sessionPtr = m_session.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  auto role = sessionPtr->GetRole();
  switch (role) {
    case ROLE::NONE_SELECT:
      WRITE_LOG(logLevel::err, "{}({}) > Session Invalid Role On InGameState !!", __FUNCTION__, __LINE__);
      return;
      break;
    case ROLE::WARRIOR: {
      m_character = std::make_shared<WarriorObject>(sessionPtr);
    } break;
    case ROLE::MAGE: {
      m_character = std::make_shared<MageObject>(sessionPtr);
    } break;
    case ROLE::TANKER: {
      m_character = std::make_shared<TankerObject>(sessionPtr);
    } break;
    case ROLE::ARCHER: {
      m_character = std::make_shared<ArcherObject>(sessionPtr);
    } break;
    default:
      break;
  }
}

void InGameState::UpdateState() {
  m_character->Update();
}

void InGameState::ExitState() {
  auto sessionPtr = m_session.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  DreamWorld::CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::GAME_END_OK));
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
  sessionPtr->SetRole(ROLE::NONE_SELECT);
  sessionPtr->SetPosition(-12341251.0f, -2351235.0f);
}

}  // namespace Stress