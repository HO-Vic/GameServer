#include "pch.h"
#include "CharacterObject.h"
#include <random>
#include <DirectXMath.h>
#include "../CoolTimeEventCtrl/SkillTimeCtrl.h"
#include "../CoolTimeEventCtrl/CoolAndDurationCtrl.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"
#include "../Session/Session.h"

namespace Stress {
CharacterObject::CharacterObject(std::shared_ptr<Session>& sessionPtr)
    : m_sessionPtr(sessionPtr), m_isChangeBoss(false), m_isGameEnd(false) {
  m_coolTimeCtrl = std::make_shared<SkillTimeCtrl>();
  m_coolTimeCtrl->InsertCoolSkill(MOVE, SkillTimeCtrl::MS(250));
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::uniform_int_distribution changeBossStage(30, 60);
  // std::uniform_int_distribution changeBossStage(2, 3);

  int randomBoss = changeBossStage(dre);

  std::uniform_int_distribution gameEnd(randomBoss + 30, randomBoss + 60);
  int randomGameEnd = gameEnd(dre);

  std::chrono::seconds randomBossTime(randomBoss);
  std::chrono::seconds randomgameEndTime(randomGameEnd);

  m_changeBossTime = std::chrono::high_resolution_clock::now() + randomBossTime;
  m_gameEndTime = std::chrono::high_resolution_clock::now() + randomgameEndTime;
}

void CharacterObject::Update() {
  auto nowTime = std::chrono::high_resolution_clock::now();
  if (!m_isChangeBoss && nowTime > m_changeBossTime) {
    auto sessionPtr = m_sessionPtr.lock();
    if (nullptr == sessionPtr) {
      return;
    }
    DreamWorld::CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS));
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
    m_isChangeBoss = true;
  }

  if (!m_isGameEnd && nowTime > m_gameEndTime) {
    auto sessionPtr = m_sessionPtr.lock();
    if (nullptr == sessionPtr) {
      return;
    }
    DreamWorld::CLIENT_PACKET::NotifyPacket sendPacket(static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::TEST_GAME_END));
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
    m_isGameEnd = true;
  }

  Move();
  SendAttackPacket();
}

void CharacterObject::Move() {
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::uniform_int_distribution rotate(0, 1);

  auto sessionPtr = m_sessionPtr.lock();

  int randRotate = rotate(dre);
  if (randRotate) {
    DreamWorld::CLIENT_PACKET::RotatePacket sendPacket(ROTATE_AXIS::Y, 1.0f, static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::ROTATE));
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
    // SendRotatePacket();
  }
  auto moveCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(MOVE);
  if (!moveCoolTime->IsAbleExecute()) {
    return;
  }

  DIRECTION randDIr = GetRandomDirection();
  if (DIRECTION::IDLE == randDIr) {
    DreamWorld::CLIENT_PACKET::StopPacket sendPacket{};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
    // SendStopPacket();
    return;
  }
  std::uniform_int_distribution keyDownRandom(0, 1);
  int keyDown = keyDownRandom(dre);
  if (1 == keyDown) {
    DreamWorld::CLIENT_PACKET::MovePacket sendPacket(randDIr, static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::MOVE_KEY_DOWN));
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
    return;
  }
  DreamWorld::CLIENT_PACKET::MovePacket sendPacket(randDIr, static_cast<unsigned char>(DreamWorld::CLIENT_PACKET::TYPE::MOVE_KEY_UP));
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
  // SendMovePacket(randDIr, keyDown);
}

DIRECTION CharacterObject::GetRandomDirection() {
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::uniform_int_distribution directionRandom(-1, 3);

  int randomDirection = directionRandom(dre);
  if (-1 == randomDirection) {
    return DIRECTION::IDLE;
  }

  if (0 == randomDirection) {
    return DIRECTION::RIGHT;
  }
  if (1 == randomDirection) {
    return DIRECTION::BACK;
  }
  if (2 == randomDirection) {
    return DIRECTION::FRONT;
  }
  return DIRECTION::LEFT;
}

WarriorObject::WarriorObject(std::shared_ptr<Session>& sessionPtr)
    : CharacterObject(sessionPtr) {
  m_coolTimeCtrl->InsertCoolSkill(SKILL_Q, SkillTimeCtrl::MS(7'100));
  m_coolTimeCtrl->InsertCoolSkill(ATTACK, SkillTimeCtrl::MS(2'100));
}

void WarriorObject::SendAttackPacket() {
  auto sessionPtr = m_sessionPtr.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  auto attackCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(ATTACK);
  auto skillCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_Q);

  if (attackCoolTime->IsAbleExecute()) {
    // attack: look, power => PlayerPowerAttackPacket
    DreamWorld::CLIENT_PACKET::PlayerPowerAttackPacket sendPacket{DirectX::XMFLOAT3(0, 0, 1), 0};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime->IsAbleExecute()) {
    // skill: look => float Data Skill Packet
    DreamWorld::CLIENT_PACKET::FloatDataSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q), DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  }
}

TankerObject::TankerObject(std::shared_ptr<Session>& sessionPtr)
    : CharacterObject(sessionPtr) {
  m_coolTimeCtrl->InsertDurationSkill(SKILL_Q, SkillTimeCtrl::MS(15'100), SkillTimeCtrl::MS(7'100));
  m_coolTimeCtrl->InsertCoolSkill(SKILL_E, SkillTimeCtrl::MS(10'100));
  m_coolTimeCtrl->InsertCoolSkill(ATTACK, SkillTimeCtrl::MS(2'100));
}

void TankerObject::SendAttackPacket() {
  auto sessionPtr = m_sessionPtr.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  auto attackCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(ATTACK);
  auto skillCoolTime_Q = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_Q);
  auto skillCoolTime_E = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_E);

  if (attackCoolTime->IsAbleExecute()) {
    // attack look => PlayerCommonAttackPacket
    DreamWorld::CLIENT_PACKET::PlayerCommonAttackPacket sendPacket{DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // skillQ: notify -> CommonSkillPacket
    DreamWorld::CLIENT_PACKET::CommonSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // Skill E: look -> FloatDataSkillPacket
    DreamWorld::CLIENT_PACKET::FloatDataSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  }
}

MageObject::MageObject(std::shared_ptr<Session>& sessionPtr)
    : CharacterObject(sessionPtr) {
  m_coolTimeCtrl->InsertDurationSkill(SKILL_Q, SkillTimeCtrl::MS(15'100), SkillTimeCtrl::MS(10'100));
  m_coolTimeCtrl->InsertCoolSkill(SKILL_E, SkillTimeCtrl::MS(10'100));
  m_coolTimeCtrl->InsertCoolSkill(ATTACK, SkillTimeCtrl::MS(2'100));
}

void MageObject::SendAttackPacket() {
  auto sessionPtr = m_sessionPtr.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  auto attackCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(ATTACK);
  auto skillCoolTime_Q = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_Q);
  auto skillCoolTime_E = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_E);

  if (attackCoolTime->IsAbleExecute()) {
    // attack look => PlayerCommonAttackPacket
    DreamWorld::CLIENT_PACKET::PlayerCommonAttackPacket sendPacket{DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // skill Q: notify => CommonSkillPacket
    DreamWorld::CLIENT_PACKET::CommonSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // skill e: position => FloatDataSkillPacket
    DreamWorld::CLIENT_PACKET::FloatDataSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  }
}

ArcherObject::ArcherObject(std::shared_ptr<Session>& sessionPtr)
    : CharacterObject(sessionPtr) {
  m_coolTimeCtrl->InsertCoolSkill(SKILL_Q, SkillTimeCtrl::MS(10'100));
  m_coolTimeCtrl->InsertCoolSkill(SKILL_E, SkillTimeCtrl::MS(15'100));
  m_coolTimeCtrl->InsertCoolSkill(ATTACK, SkillTimeCtrl::MS(2'100));
}

void ArcherObject::SendAttackPacket() {
  auto sessionPtr = m_sessionPtr.lock();
  if (nullptr == sessionPtr) {
    return;
  }
  auto attackCoolTime = m_coolTimeCtrl->GetSkillCoolInfo(ATTACK);
  auto skillCoolTime_Q = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_Q);
  auto skillCoolTime_E = m_coolTimeCtrl->GetSkillCoolInfo(SKILL_E);

  if (attackCoolTime->IsAbleExecute()) {
    // attack: SendPowerAttackExecute
    DreamWorld::CLIENT_PACKET::PlayerPowerAttackPacket sendPacket{DirectX::XMFLOAT3(0, 0, 1), 1};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // skillQ: look -> FloatDataSkillPacket
    DreamWorld::CLIENT_PACKET::FloatDataSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q), DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  } else if (skillCoolTime_Q->IsAbleExecute()) {
    // Skill E: position -> FloatDataSkillPacket
    DreamWorld::CLIENT_PACKET::FloatDataSkillPacket sendPacket{static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), DirectX::XMFLOAT3(0, 0, 1)};
    sessionPtr->DoSend(&sendPacket, sendPacket.size);
  }
}
}  // namespace Stress
