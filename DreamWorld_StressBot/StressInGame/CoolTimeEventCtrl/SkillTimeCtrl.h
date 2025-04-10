#pragma once
#include <chrono>
#include <memory>
#include <unordered_map>
#include <cstdint>

namespace Stress {
class CoolAndDurationCtrl;

class SkillTimeCtrl {
 public:
  using MS = std::chrono::milliseconds;
  using SEC = std::chrono::seconds;
  using MIN = std::chrono::minutes;

  const bool InsertCoolSkill(const uint8_t skillId, const MS& coolTime);

  const bool InsertDurationSkill(const uint8_t skillId, const MS& coolTime, const MS& durationTime);

  std::shared_ptr<CoolAndDurationCtrl> GetSkillCoolInfo(const uint8_t skillId);

 private:
  std::unordered_map<uint8_t, std::shared_ptr<CoolAndDurationCtrl>> m_coolDurationDatas;
};
}  // namespace Stress