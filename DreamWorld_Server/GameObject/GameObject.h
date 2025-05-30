#pragma once
#include "../DreamWorldDefine.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;
namespace DreamWorld {

class RoomBase;
class GameObject : public std::enable_shared_from_this<GameObject> {
 public:
  GameObject() = delete;
  GameObject(const float& boundingSize, std::shared_ptr<RoomBase>& roomRef);
  virtual ~GameObject() = default;
  virtual void Update() = 0;

  void Rotate(const ROTATE_AXIS& axis, const float& angle);

  bool IsCollide(const DirectX::BoundingSphere& otherCollision);
  bool IsCollide(const DirectX::BoundingOrientedBox& otherCollision);

  void SetPosition(const DirectX::XMFLOAT3& newPosition);
  const DirectX::XMFLOAT3 GetPosition() const;

  const DirectX::XMFLOAT3 GetLookVector() const;
  const DirectX::XMFLOAT3 GetRightVector() const;

  const DirectX::BoundingSphere& GetCollision() const;
  const XMFLOAT3 GetFromVector(const XMFLOAT3& from) const;
  const XMFLOAT3 GetToVector(const XMFLOAT3& to) const;
  const float GetDistance(const XMFLOAT3& other) const;
  const float GetDistance(const std::shared_ptr<const GameObject>& other) const;

  const float GetBetweenAngleCosValue(const XMFLOAT3& position) const;

 protected:
  virtual void UpdateCollision();
  void SetLook(const XMFLOAT3& lookVector);

 protected:
  // 11 12 13 r
  // 21 22 23 u
  // 31 32 33 f
  // 41 42 43 pos
  DirectX::XMFLOAT4X4 m_worldMatrix;

  DirectX::BoundingSphere m_collisionSphere;

  // Room의 멤버 변수여서 먼저 소멸자 호출되니, 문제 없을거로 생각 됨. => 잘못된 생각을 했네.
  // 얘 때문에 소멸자가 안불려서 메모리 해제가 안되는 상황이 발생하게 되네
  std::weak_ptr<RoomBase> m_roomWeakRef;
};

class MoveObject : public GameObject {
 public:
  MoveObject(const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef);
  const std::chrono::high_resolution_clock::time_point GetLastUpdateTime() const;
  float GetElapsedLastUpdateTime();
  void UpdateLastUpdateTime();

 protected:
  std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

  float m_moveSpeed;
};

class LiveObject : public MoveObject {
 public:
  LiveObject() = delete;
  LiveObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef);
  ~LiveObject() = default;

  virtual void Attacked(const float& damage);

  const float GetHp() const;
  const float GetMaxHp() const;
  const bool IsAlive() const {
    return m_isAlive;
  }

 protected:
  std::optional<std::pair<bool, XMFLOAT3>> CollideLiveObject(const XMFLOAT3& nextPosition, const XMFLOAT3& moveVector, const float& elapsedTime, const bool& isSlidingPosition);

  virtual const XMFLOAT3 GetCommonNextPosition(const float& elapsedTime) = 0;

  static DirectX::BoundingOrientedBox GetMeleeAttackJudgeBox(const XMFLOAT3& startPosition, const XMFLOAT3& forwardVector, const float& offsetLegth, const float& width, const float& attackLegth, const float& height);

 protected:
  std::atomic_bool m_isAlive;
  std::atomic<float> m_hp;
  float m_maxHp;

  XMFLOAT3 m_moveVector;
};
}  // namespace DreamWorld
