#pragma once
#include <chrono>

namespace Stress {
class CoolAndDurationCtrl {
 public:
  using MS = std::chrono::milliseconds;
  using TIME = std::chrono::high_resolution_clock::time_point;

 public:
  CoolAndDurationCtrl() = delete;
  CoolAndDurationCtrl(const MS& coolTime)
      : m_coolTime(coolTime), m_durationTime(0), m_lastExecTime(std::chrono::high_resolution_clock::now() - coolTime) {
  }

  CoolAndDurationCtrl(const MS& coolTime, const MS& durationTime)
      : m_coolTime(coolTime), m_durationTime(durationTime), m_lastExecTime(std::chrono::high_resolution_clock::now() - coolTime) {
  }

  const bool IsAbleExecute();

  void ForceExecute();

  const TIME& GetLastExeTime() const;

  void ResetCoolTime();

  const MS& GetDurationTIme() const {
    return m_durationTime;
  }

 private:
  MS m_coolTime;
  TIME m_lastExecTime;
  MS m_durationTime;
};
}  // namespace Stress
