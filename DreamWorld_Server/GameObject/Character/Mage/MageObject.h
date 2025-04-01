#pragma once
#include "../PCH/stdafx.h"
#include "../ChracterObject.h"
#include "../PlayerSkill.h"

class MageObject;
namespace DreamWorld {
class MageObject;
namespace MageSkill {
// 어떤 위치에 번개, 힐 스킬 => 2개

class PositionSkill : public PositionSkillBase {
 public:
  PositionSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& position)
      : m_mageRef(mageRef), PositionSkillBase(position) {
  }
  std::shared_ptr<MageObject> m_mageRef;
};

class DurationSKill : public CommonDurationSkill_MILSEC {
 public:
  DurationSKill(std::shared_ptr<MageObject> mageRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
      : m_mageRef(mageRef), CommonDurationSkill_MILSEC(durationTime) {
  }
  std::shared_ptr<MageObject> m_mageRef;
};

class DirectionSkill : public DirectionSkillBase {
 public:
  DirectionSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& direction)
      : m_mageRef(mageRef), DirectionSkillBase(direction) {
  }
  std::shared_ptr<MageObject> m_mageRef;
};

class ThunderSkill : public PositionSkill {
 public:
  ThunderSkill(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& position)
      : PositionSkill(mageRef, position) {
  }
  virtual void Execute() override;
};

class HealSkill : public DurationSKill {
 public:
  HealSkill(std::shared_ptr<MageObject> mageRef, const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime)
      : DurationSKill(mageRef, durationTime) {
  }
  virtual void Execute() override;
};

class CommonAttack : public DirectionSkill  // 연속형 공격없이 그대로 공격하면 됨.
{
 public:
  CommonAttack(std::shared_ptr<MageObject> mageRef, const XMFLOAT3& direction)
      : DirectionSkill(mageRef, direction) {
  }
  virtual void Execute() override;
};
}  // namespace MageSkill

class MageObject : public RangedCharacterObject {
 public:
  MageObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<RoomBase>& roomRef);
  ~MageObject() = default;

 public:
  virtual void SetStagePosition(const ROOM_STATE& roomState) override;

  virtual void RecvSkill(const SKILL_TYPE&) override;
  virtual void RecvSkill(const SKILL_TYPE&, const XMFLOAT3& vector3) override;
  virtual void RecvAttackCommon(const XMFLOAT3& attackDir, const int& power = 0) override;

  void ExecuteThunderSkill(const XMFLOAT3& position);
  void ExecuteHeal(const CommonDurationSkill_MILSEC::DURATION_TIME_RATIO& durationTime);
  void ExecuteCommonAttack(const XMFLOAT3& attackDir);

  void ExecHeal();

 private:
  bool m_healing;
};
}  // namespace DreamWorld
