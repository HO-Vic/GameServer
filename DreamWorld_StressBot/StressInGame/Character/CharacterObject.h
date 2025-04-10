#pragma once
#include <memory>
#include <cstdint>
#include "../../DreamWorld_Server/DreamWorldDefine.h"

namespace Stress {
class Session;
class SkillTimeCtrl;
struct PacketHeader;

class CharacterObject : std::enable_shared_from_this<CharacterObject> {
 public:
  CharacterObject(std::shared_ptr<Session>& sessionPtr);

 protected:
  static constexpr uint8_t MOVE = 1;
  static constexpr uint8_t ATTACK = 11;
  static constexpr uint8_t SKILL_Q = 12;
  static constexpr uint8_t SKILL_E = 13;

 public:
  void Update();

 protected:
  void Move();

  virtual void SendAttackPacket() = 0;

 private:
  static DIRECTION GetRandomDirection();

 protected:
  std::shared_ptr<SkillTimeCtrl> m_coolTimeCtrl;
  std::weak_ptr<Session> m_sessionPtr;

  // 테스트 봇이 스스로 보스맵으로, 게임 종료 할 수 있게
  bool m_isChangeBoss;
  bool m_isGameEnd;
  std::chrono::high_resolution_clock::time_point m_changeBossTime;
  std::chrono::high_resolution_clock::time_point m_gameEndTime;
};

class WarriorObject : public CharacterObject {
 public:
  WarriorObject(std::shared_ptr<Session>& sessionPtr);

 protected:
  virtual void SendAttackPacket() override;
};

class TankerObject : public CharacterObject {
 public:
  TankerObject(std::shared_ptr<Session>& sessionPtr);

 protected:
  virtual void SendAttackPacket() override;
};

class MageObject : public CharacterObject {
 public:
  MageObject(std::shared_ptr<Session>& sessionPtr);

 protected:
  virtual void SendAttackPacket() override;
};

class ArcherObject : public CharacterObject {
 public:
  ArcherObject(std::shared_ptr<Session>& sessionPtr);

 protected:
  virtual void SendAttackPacket() override;
};
}  // namespace Stress
