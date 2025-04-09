#pragma once
#include <chrono>
#include "../Job/Job.h"

namespace sh::Utility {
using MS = std::chrono::milliseconds;

using _chrono_clock = std::chrono::steady_clock;

class TimerJob
    : public Job {
 public:
  TimerJob() = default;

  TimerJob(_chrono_clock::time_point wakeTime, Job::Caller&& func);

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
}  // namespace sh::Utility
