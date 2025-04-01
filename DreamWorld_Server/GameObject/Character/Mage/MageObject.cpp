#include "stdafx.h"
#include "MageObject.h"
#include <Utility/Job/Job.h>
#include "../Room/Room.h"
#include "../Room/RoomBase.h"
#include "../../EventController/EventController.h"
#include "../../EventController/CoolDownEventBase.h"
#include "../../EventController/DurationEvent.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include "../LogManager/LogManager.h"
#include "../ObjectPools.h"
#include "../Server/MsgProtocol.h"
#include "../Timer/TimerJob.h"

namespace DreamWorld {
MageObject::MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef)
    : RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::MAGE), m_healing(false) {
  m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(10'000));
  m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
  /*
  m_maxHp = m_hp = 500;
  m_attackDamage = 50;
  */
}

void MageObject::SetStagePosition(const ROOM_STATE& roomState) {
  if (ROOM_STATE::ROOM_COMMON == roomState) {
    this->SetPosition(XMFLOAT3(-1370.45f, 0, -1450.89f));
  } else {
    this->SetPosition(XMFLOAT3(20, 0, -285));
  }
  // SetPosition(XMFLOAT3(-77.7, 0, -164.7));
}

void MageObject::RecvSkill(const SKILL_TYPE& type) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  if (SKILL_TYPE::SKILL_TYPE_Q == type) {
    m_healing = true;
    auto durationEventData = std::static_pointer_cast<DurationEvent>(m_skillCtrl->GetEventData(SKILL_Q));
    InsertJobQ(std::make_unique<TimerJob>(chrono_clock::now() + MS(500),  // 바로 실행할 수 있게
                                          [this]() {
                                            ExecHeal();
                                          }));
    InsertJobQ(std::make_unique<TimerJob>(chrono_clock::now() + durationEventData->GetDurationTIme(),  // 바로 실행할 수 있게
                                          [this]() {
                                            m_healing = false;
                                          }));
    /*roomRef->InsertJob(ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
      MageSkill::HealSkill skill(std::static_pointer_cast<MageObject>(shared_from_this()), durationEventData->GetDurationTIme());
      skill.Execute();
    }));*/
  } else {
    WRITE_LOG(logLevel::critical, "{}({}) > Mage Can not Execute Skill E, But Executed", __FUNCTION__, __LINE__);
  }
}

void MageObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  if (SKILL_TYPE::SKILL_TYPE_E == type) {
    roomRef->InsertJob(ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
      MageSkill::ThunderSkill skill(std::static_pointer_cast<MageObject>(shared_from_this()), vector3);
      skill.Execute();
    }));
  } else {
    WRITE_LOG(logLevel::critical, "{}({}) > Mage Can not Execute Skill Q, But Executed", __FUNCTION__, __LINE__);
  }
}

void MageObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr != roomRef) {
    roomRef->InsertJob(ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
      MageSkill::CommonAttack commonAttackEvent(std::static_pointer_cast<MageObject>(shared_from_this()), attackDir);
      commonAttackEvent.Execute();
    }));
  }
}

void MageObject::ExecuteThunderSkill(const XMFLOAT3& position) {
  auto roomRef = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
  if (nullptr == roomRef) {
    return;
  }
  DreamWorld::SERVER_PACKET::ShootingObject sendPacket(static_cast<char>(SERVER_PACKET::TYPE::EXECUTE_LIGHTNING), position);
  roomRef->Broadcast(&sendPacket);

  static constexpr float THUNDER_RANGE = 35.0f;
  static constexpr float THUNDER_DAMAGE = 150.0f;
  auto enermyData = roomRef->GetMonsters();
  for (auto& monster : enermyData) {
    float monsterDistance = monster->GetDistance(position);
    if (THUNDER_RANGE > monsterDistance) {
      monster->Attacked(THUNDER_DAMAGE);
    }
  }
}

void MageObject::ExecuteHeal(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime) {  // 얘도 PlayerCharacter에 Tickable Ojbect
                                                                                                     // 더 이상 쓰지 않음
  using namespace std::chrono;

  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }

  /*TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
  auto startEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_START, 1ms, roomRef);
  auto endEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_HEAL_END, durationTime, roomRef);

  timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(startEvent));
  timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(endEvent));*/
}

void MageObject::ExecuteCommonAttack(const XMFLOAT3& attackDir) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  auto currentPosition = GetPosition();
  currentPosition.y += 8.0f;
  std::shared_ptr<ProjectileObject> iceLance = std::make_shared<IceLanceBall>(roomRef, currentPosition, attackDir);
  roomRef->InsertProjectileObject(iceLance);

  DreamWorld::SERVER_PACKET::ShootingObject sendPacket(static_cast<char>(SERVER_PACKET::TYPE::SHOOTING_ICE_LANCE), attackDir);
  roomRef->Broadcast(&sendPacket);
}

void MageObject::ExecHeal() {
  static constexpr float HEAL_HP = 75.0f;
  if (!m_healing) {
    return;
  }
  auto roomPtr = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
  if (nullptr == roomPtr) {
    return;
  }
  DreamWorld::SERVER_PACKET::NotifyHealPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::NOTIFY_HEAL_HP));
  int i = 0;
  auto characters = roomPtr->GetCharacters(true);
  for (auto& character : characters) {
    auto characterPtr = std::static_pointer_cast<CharacterObject>(character);
    characterPtr->Heal(HEAL_HP);
    sendPacket.applyHealPlayerInfo[i].role = characterPtr->GetRole();
    sendPacket.applyHealPlayerInfo[i].hp = character->GetHp();
    ++i;
  }
  roomPtr->Broadcast(&sendPacket);
  // 1초 뒤에 다시 이벤트 삽입
  InsertJobQ(std::make_unique<TimerJob>(chrono_clock::now() + MS(1000),  // 바로 실행할 수 있게
                                        [this]() {
                                          ExecHeal();
                                        }));
}

void MageSkill::ThunderSkill::Execute() {
  m_mageRef->ExecuteThunderSkill(position);
}

void MageSkill::HealSkill::Execute() {
  m_mageRef->ExecuteHeal(durationTime);
}

void MageSkill::CommonAttack::Execute() {
  m_mageRef->ExecuteCommonAttack(direction);
}
}  // namespace DreamWorld
