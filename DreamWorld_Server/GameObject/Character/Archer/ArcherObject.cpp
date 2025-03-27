#include "stdafx.h"
#include "ArcherObject.h"
#include "../Room/Room.h"
#include "../../EventController/EventController.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include <Utility/Job/Job.h>
#include <Utility/Pool/ObjectPool.h>
#include "../ObjectPools.h"
#include "../Server/MsgProtocol.h"

namespace DreamWorld {
ArcherObject::ArcherObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef)
    : RangedCharacterObject(maxHp, moveSpeed, boundingSize, 80.0f, roomRef, ROLE::ARCHER) {
  m_skillCtrl->InsertCoolDownEventData(SKILL_Q, EventController::MS(10'000));
  m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(15'000));
  /*
  m_maxHp = m_hp = 400;
  m_attackDamage = 120;
  */
}

void ArcherObject::SetStagePosition(const ROOM_STATE& roomState) {
  if (ROOM_STATE::ROOM_COMMON == roomState) {
    this->SetPosition(XMFLOAT3(-1340.84f, 0, -1520.93f));
  } else {
    this->SetPosition(XMFLOAT3(123, 0, -293));
  }
  // SetPosition(XMFLOAT3(-193, 0, -52));
}

void ArcherObject::RecvSkill(const SKILL_TYPE& type) {
  spdlog::critical("ArcherObject::RecvSkill(const SKILL_TYPE&) - Non Use Func");
}

void ArcherObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;  // Already Room Expire;
  }
  if (SKILL_TYPE::SKILL_TYPE_Q == type) {
    roomRef->InsertJob(
        DreamWorld::ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(std::move([=]() {
          ArcherSKill::TripleArrow skill(std::static_pointer_cast<ArcherObject>(shared_from_this()), vector3);
          skill.Execute();
        })));
  } else if (SKILL_TYPE::SKILL_TYPE_E == type) {
    roomRef->InsertJob(
        DreamWorld::ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(std::move([=]() {
          ArcherSKill::RainArrow skill(std::static_pointer_cast<ArcherObject>(shared_from_this()), vector3);
          skill.Execute();
        })));
  }
}

void ArcherObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr != roomRef) {
    roomRef->InsertJob(
        DreamWorld::ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(std::move([=]() {
          ArcherSKill::CommonAttack skill(std::static_pointer_cast<ArcherObject>(shared_from_this()), attackDir, power);
          skill.Execute();
        })));
  }
}

void ArcherObject::ExecuteTripleArrow(const XMFLOAT3& direction) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  //  .
  // . . <= 이런 형태
  auto currentPosition = GetPosition();
  auto rightVector = GetRightVector();

  for (int i = 0; i < 3; ++i) {
    XMFLOAT3 startPosition = currentPosition;
    startPosition.y = 6.0f + float(i % 2) * 4.0f;
    startPosition = Vector3::Add(startPosition, rightVector, float(1 - i) * 4.0f);
    startPosition = Vector3::Add(startPosition, direction);
    std::shared_ptr<ProjectileObject> tripleArrowObject = std::make_shared<TripleArrowObject>(roomRef, startPosition, direction);
    roomRef->InsertProjectileObject(tripleArrowObject);
  }
  DreamWorld::SERVER_PACKET::ShootingObject sendPacket(static_cast<char>(SERVER_PACKET::TYPE::SHOOTING_ARROW), direction);
  roomRef->Broadcast(&sendPacket);
}

void ArcherObject::ExecuteRainArrow(const XMFLOAT3& position) {  // 이런 타이머적인 스킬은 player Tickable Object 둬서 업데이트 처리하자
  using namespace std::chrono;
  static constexpr seconds SKY_ARROW_ATTACK_TIME = seconds(1);

  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }

  m_attackRainArrowPosition = position;
  // TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
  // auto skyArrowEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_RAIN_ARROW_ATTACK, SKY_ARROW_ATTACK_TIME, roomRef);
  // timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(skyArrowEvent));
}

void ArcherObject::ExecuteCommonAttack(const XMFLOAT3& direction, const int& power) {
  auto currentPosition = GetPosition();
  currentPosition.y += 8.0f;
  float attackDamage = CommonArrowObject::POWER_ARROW_LEVEL1_SPEED;
  float Speed = CommonArrowObject::POWER_ARROW_LEVEL1_DAMAGE;
  switch (power) {
    case 0: {
      attackDamage = CommonArrowObject::POWER_ARROW_LEVEL1_SPEED;
      Speed = CommonArrowObject::POWER_ARROW_LEVEL1_DAMAGE;
    } break;
    case 1: {
      attackDamage = CommonArrowObject::POWER_ARROW_LEVEL2_SPEED;
      Speed = CommonArrowObject::POWER_ARROW_LEVEL2_DAMAGE;
    } break;
    case 2: {
      attackDamage = CommonArrowObject::POWER_ARROW_LEVEL3_SPEED;
      Speed = CommonArrowObject::POWER_ARROW_LEVEL3_DAMAGE;
    } break;
    default:
      break;
  }
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  std::shared_ptr<ProjectileObject> arrowObject = std::make_shared<CommonArrowObject>(Speed, roomRef, currentPosition, direction, attackDamage);
  roomRef->InsertProjectileObject(arrowObject);
  SERVER_PACKET::ShootingObject sendPacket(static_cast<char>(SERVER_PACKET::TYPE::SHOOTING_ARROW), direction);
  roomRef->Broadcast(&sendPacket);
}

void ArcherObject::AttackRainArrow() {
  m_attackRainArrowPosition;
  static constexpr float RAIN_ARROW_RANGE = 40.0f;
  static constexpr float RAIN_ARROW_DAMAGE = 120.0f;

  auto roomRef = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
  if (nullptr == roomRef) {
    return;
  }

  auto enermyData = roomRef->GetMonsters();
  for (auto& monster : enermyData) {
    float monsterDistance = monster->GetDistance(m_attackRainArrowPosition);
    if (RAIN_ARROW_RANGE > monsterDistance) {
      monster->Attacked(RAIN_ARROW_DAMAGE);
    }
  }
}

void ArcherSKill::TripleArrow::Execute() {
  archerRef->ExecuteTripleArrow(direction);
}

void ArcherSKill::RainArrow::Execute() {
  archerRef->ExecuteRainArrow(position);
}

void ArcherSKill::CommonAttack::Execute() {
  archerRef->ExecuteCommonAttack(direction, power);
}
}  // namespace DreamWorld
