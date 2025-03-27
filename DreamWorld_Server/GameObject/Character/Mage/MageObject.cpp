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

namespace DreamWorld {
MageObject::MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef)
    : RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::MAGE) {
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
    auto durationEventData = std::static_pointer_cast<DurationEvent>(m_skillCtrl->GetEventData(SKILL_Q));
    roomRef->InsertJob(ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
      MageSkill::HealSkill skill(std::static_pointer_cast<MageObject>(shared_from_this()), durationEventData->GetDurationTIme());
      skill.Execute();
    }));
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
