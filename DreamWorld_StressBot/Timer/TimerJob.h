#pragma once
#include <chrono>
#include <Utility/Job/Job.h>

namespace Stress {
using MS = std::chrono::milliseconds;

using _chrono_clock = std::chrono::steady_clock;

class TimerJob
    : public sh::Utility::Job {
 public:
  TimerJob() = default;

  TimerJob(_chrono_clock::time_point wakeTime, sh::Utility::Job::Caller&& func);

  // priorityQueue에서 우선 순위 판단을 위한 operator
  constexpr bool operator<(const TimerJob& other) const {
    return (m_wakeupTime > other.m_wakeupTime);
  }

  bool IsReady();

  const MS GetRestTimeForReady() const;

  const int64_t GetRestTime() const;

  const int64_t GetWakeTime() const;

 protected:
  _chrono_clock::time_point m_wakeupTime;
  int64_t m_restTime;
};
}  // namespace Stress
