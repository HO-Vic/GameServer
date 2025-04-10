#include "pch.h"
#include "SkillTimeCtrl.h"
#include "CoolAndDurationCtrl.h"

namespace Stress {
const bool SkillTimeCtrl::InsertCoolSkill(const uint8_t skillId, const MS& coolTime) {
  if (m_coolDurationDatas.count(skillId)) {
    return false;
  }
  auto coolDownEvent = std::make_shared<CoolAndDurationCtrl>(coolTime);
  m_coolDurationDatas.emplace(skillId, coolDownEvent);
  return true;
}

const bool SkillTimeCtrl::InsertDurationSkill(const uint8_t skillId, const MS& coolTime, const MS& durationTime) {
  if (m_coolDurationDatas.count(skillId)) {
    return false;
  }
  auto durationEvent = std::make_shared<CoolAndDurationCtrl>(coolTime, durationTime);
  m_coolDurationDatas.emplace(skillId, durationEvent);
  return true;
}

std::shared_ptr<CoolAndDurationCtrl> SkillTimeCtrl::GetSkillCoolInfo(const uint8_t skillId) {
  if (!m_coolDurationDatas.count(skillId)) {
    return nullptr;
  }
  return m_coolDurationDatas[skillId];
}
}  // namespace Stress
