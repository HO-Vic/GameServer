#include "stdafx.h"
#include "ProjectileObject.h"
#include "../Room/RoomBase.h"
#include "../Character/ChracterObject.h"
#include "../Monster/MonsterObject.h"
#include "../Room/Room.h"

namespace DreamWorld {
ProjectileObject::ProjectileObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime)
    : MoveObject(moveSpeed, boundingSize, roomRef), m_attackDamage(attackDamage), m_destroyTime(std::chrono::high_resolution_clock::now() + lifeTime), m_isCollide(false) {
  SetPosition(startPosition);
  SetLook(directionVector);
}

void ProjectileObject::Update() {
  if (EnermyObjectsAttackCheck()) {
    m_isCollide = true;
    return;
  }
  // 위치 업데이트
  float elapsedTime = GetElapsedLastUpdateTime();
  XMFLOAT3 nextPosition = GetPosition();
  XMFLOAT3 directionVector = GetLookVector();
  nextPosition = Vector3::Add(nextPosition, directionVector, elapsedTime * m_moveSpeed);
  SetPosition(nextPosition);
}

const bool ProjectileObject::IsDestroy() const {
  auto currentTime = std::chrono::high_resolution_clock::now();
  return m_isCollide || (currentTime > m_destroyTime);
}

void ProjectileObject::UpdateCollision() {
  m_collisionSphere.Center = GetPosition();
}

void ProjectileObject::Attack(std::shared_ptr<LiveObject> damagedObject) {
  damagedObject->Attacked(m_attackDamage);
}

PlayerProjectileObject::PlayerProjectileObject(const float& moveSpeed, std::shared_ptr<RoomBase>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime)
    : ProjectileObject(moveSpeed, 4.0f, roomRef, startPosition, directionVector, attackDamage, lifeTime) {
}

bool PlayerProjectileObject::EnermyObjectsAttackCheck() {
  auto roomBaseRef = m_roomWeakRef.lock();
  if (nullptr == roomBaseRef) {
    return false;
  }
  auto roomRef = std::static_pointer_cast<Room>(roomBaseRef);
  auto enermyVector = roomRef->GetMonsters();
  for (auto& enermy : enermyVector) {
    // const bool isCollide = enermy->GetDistance(shared_from_this()) < 12.0f;
    const bool isCollide = enermy->IsCollide(m_collisionSphere);
    if (isCollide) {
      Attack(enermy);
      return true;
    }
  }
  return false;
}

MonsterProjectileObject::MonsterProjectileObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage, const SEC& lifeTime)
    : ProjectileObject(moveSpeed, 10.0f, roomRef, startPosition, directionVector, attackDamage, lifeTime) {
}

bool MonsterProjectileObject::EnermyObjectsAttackCheck() {
  auto roomBaseRef = m_roomWeakRef.lock();
  if (nullptr == roomBaseRef) {
    return false;
  }
  auto roomRef = std::static_pointer_cast<Room>(roomBaseRef);

  auto playerVector = roomRef->GetCharacters();
  for (auto& player : playerVector) {
    Attack(player);
    return true;
  }
  return false;
}

CommonArrowObject::CommonArrowObject(const float& moveSpeed, std::shared_ptr<RoomBase>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector, const float& attackDamage)
    : PlayerProjectileObject(moveSpeed, roomRef, startPosition, directionVector, attackDamage, ARROW_LIFE_TIME) {
}

TripleArrowObject::TripleArrowObject(std::shared_ptr<RoomBase> roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector)
    : PlayerProjectileObject(TRIPLE_ARROW_SPEED, roomRef, startPosition, directionVector, TRIPLE_ARROW_DAMAGE, TRIPE_ARROW_LIFE_TIME) {
}

IceLanceBall::IceLanceBall(std::shared_ptr<RoomBase>& roomRef, const XMFLOAT3& startPosition, const XMFLOAT3& directionVector)
    : PlayerProjectileObject(IceLance_SPEED, roomRef, startPosition, directionVector, IceLance_DAMAGE, ICE_LANCE_LIFE_TIME) {
}
}  // namespace DreamWorld
