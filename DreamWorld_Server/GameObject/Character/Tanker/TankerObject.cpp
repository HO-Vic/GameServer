#include "stdafx.h"
#include "TankerObject.h"
#include "../Room/Room.h"
#include "../../EventController/CoolDownEventBase.h"
#include "../../EventController/DurationEvent.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../ObjectPools.h"
#include "../Server/MsgProtocol.h"
#include "../Timer/TimerJob.h"

namespace DreamWorld {
TankerObject::TankerObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef)
    : MeleeCharacterObject(maxHp, moveSpeed, boundingSize, 50.0f, roomRef, ROLE::TANKER) {
  m_skillCtrl->InsertDurationEventData(SKILL_Q, EventController::MS(15'000), EventController::MS(7'000));
  m_skillCtrl->InsertCoolDownEventData(SKILL_E, EventController::MS(10'000));
  /*
  m_maxHp = m_hp = 780;
  m_attackDamage = 80;
  */
}

void TankerObject::SetStagePosition(const ROOM_STATE& roomState) {
  if (ROOM_STATE::ROOM_COMMON == roomState) {
    this->SetPosition(XMFLOAT3(-1260.3f, 0, -1510.7f));
  } else {
    this->SetPosition(XMFLOAT3(82, 0, -223.0f));
  }
  //	SetPosition(XMFLOAT3(106.8, 0, -157));
}

void TankerObject::RecvSkill(const SKILL_TYPE& type) {
  static constexpr MS SHIELD_APPLY_TIME = MS(2400);
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }
  if (SKILL_TYPE::SKILL_TYPE_Q == type) {
    auto durationEvent = std::static_pointer_cast<DurationEvent>(m_skillCtrl->GetEventData(SKILL_Q));
    auto shieldSkill = std::make_shared<TankerSkill::ShieldSkill>(std::static_pointer_cast<TankerObject>(shared_from_this()), durationEvent->GetDurationTIme());

    InsertJobQ(std::make_unique<TimerJob>(chrono_clock::now() + SHIELD_APPLY_TIME,  // 2.4초 뒤에 쉴드 적용
                                          [this]() {
                                            auto roomPtr = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
                                            if (nullptr == roomPtr) {
                                              return;
                                            }
                                            auto characters = roomPtr->GetCharacters(true);
                                            for (auto& character : characters) {
                                              auto characterPtr = std::static_pointer_cast<CharacterObject>(character);
                                              characterPtr->SetShield(true);
                                              DreamWorld::SERVER_PACKET::NotifyPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_START));
                                              roomPtr->Broadcast(&sendPacket);
                                            }
                                          }));
    InsertJobQ(std::make_unique<TimerJob>(chrono_clock::now() + SHIELD_APPLY_TIME + durationEvent->GetDurationTIme(),  // 바로 실행할 수 있게
                                          [this]() {
                                            auto roomPtr = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
                                            if (nullptr == roomPtr) {
                                              return;
                                            }
                                            auto characters = roomPtr->GetCharacters(true);
                                            for (auto& character : characters) {
                                              auto characterPtr = std::static_pointer_cast<CharacterObject>(character);
                                              characterPtr->SetShield(false);
                                              DreamWorld::SERVER_PACKET::NotifyPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_END));
                                              roomPtr->Broadcast(&sendPacket);
                                            }
                                          }));
  } else {
    spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_E");
  }
}

void TankerObject::RecvSkill(const SKILL_TYPE& type, const XMFLOAT3& vector3) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }

  if (SKILL_TYPE::SKILL_TYPE_E == type) {
    roomRef->InsertJob(
        ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
          TankerSkill::ThunderHammerSkill skill(std::static_pointer_cast<TankerObject>(shared_from_this()), vector3);
          skill.Execute();
        }));
  } else {
    spdlog::critical("TankerObject::RecvSkill(const SKILL_TYPE& ) - Non Use SKILL_Q");
  }
}

void TankerObject::RecvAttackCommon(const XMFLOAT3& attackDir, const int& power) {
  auto roomRef = m_roomWeakRef.lock();
  if (nullptr != roomRef) {
    roomRef->InsertJob(
        ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
          TankerSkill::CommonAttack skill(std::static_pointer_cast<TankerObject>(shared_from_this()), attackDir);
          skill.Execute();
        }));
  }
}

void TankerObject::ExecuteHammerSkill(const XMFLOAT3& direction) {
  static constexpr float FIRST_CHECK_DISTANCE = 70.0f;
  static constexpr float HAMMER_SKILL_LENGTH = 55.0f;
  static constexpr float HAMMER_SKILL_VALID_EULER = 17.0f;
  static const float HAMMER_VALID_COS_VALUE = cos(XMConvertToRadians(HAMMER_SKILL_VALID_EULER));
  static constexpr float DAMAGE = 100.0f;

  auto roomRef = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
  if (nullptr == roomRef) {
    return;
  }

  auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, HAMMER_SKILL_LENGTH / 2.0f, 7.0f, HAMMER_SKILL_LENGTH, m_collisionSphere.Radius * 2.0f);

  auto enermyData = roomRef->GetMonsters();
  for (auto& monster : enermyData) {
    auto length = GetDistance(monster->GetPosition());
    if (length > FIRST_CHECK_DISTANCE) {
      continue;
    }
    if (attackBoudingBox.Intersects(monster->GetCollision())) {
      monster->Attacked(DAMAGE);
    }
    // float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
    // if (betweenCosValue > HAMMER_VALID_COS_VALUE) {
    // }
  }
}

void TankerObject::ExecuteShield(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime) {
  // 더 이상 쓰지 않는 함수
  using namespace std::chrono;

  auto roomRef = m_roomWeakRef.lock();
  if (nullptr == roomRef) {
    return;
  }

  /*auto applyEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, SHIELD_APPLY_TIME, roomRef);
  auto removeEventEvent = std::make_shared<TIMER::RoomEvent>(TIMER_EVENT_TYPE::EV_APPLY_SHIELD, durationTime + SHIELD_APPLY_TIME, roomRef);

  TIMER::Timer& timerRef = TIMER::Timer::GetInstance();
  timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(applyEvent));
  timerRef.InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(removeEventEvent));*/
}

void TankerObject::ExecuteCommonAttack(const XMFLOAT3& attackDir) {
  static constexpr float FIRST_CHECK_DISTANCE = 55.0f;
  static constexpr float DEFAULT_SKILL_LENGTH = 35.0f;
  static constexpr float DAMAGE = 60.0f;
  static constexpr float DEFAULT_ATTAK_VALID_EULER = 15.0f;
  static const float DEFAULT_ATTAK_VALID_COS_VALUE = cos(XMConvertToRadians(DEFAULT_ATTAK_VALID_EULER));

  auto roomRef = std::static_pointer_cast<Room>(m_roomWeakRef.lock());
  if (nullptr == roomRef) {
    return;
  }

  auto attackBoudingBox = GetMeleeAttackJudgeBox(GetPosition(), m_moveVector, DEFAULT_SKILL_LENGTH / 2.0f, 7.0f, DEFAULT_SKILL_LENGTH, m_collisionSphere.Radius * 2.0f);

  auto enermyData = roomRef->GetMonsters();
  for (auto& monster : enermyData) {
    auto length = GetDistance(monster->GetPosition());
    if (length > FIRST_CHECK_DISTANCE) {
      continue;
    }
    if (attackBoudingBox.Intersects(monster->GetCollision())) {
      monster->Attacked(DAMAGE);
    }

    // float betweenCosValue = GetBetweenAngleCosValue(monster->GetPosition());
    // if (betweenCosValue > DEFAULT_ATTAK_VALID_COS_VALUE) {
    // }
  }
}

void TankerSkill::ThunderHammerSkill::Execute() {
  tankerRef->ExecuteHammerSkill(direction);
}

void TankerSkill::ShieldSkill::Execute() {
  tankerRef->ExecuteShield(durationTime);
}

void TankerSkill::CommonAttack::Execute() {
  tankerRef->ExecuteCommonAttack(direction);
}
}  // namespace DreamWorld
