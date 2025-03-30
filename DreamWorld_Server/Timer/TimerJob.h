#pragma once
#include "../PCH/stdafx.h"
#include <Utility/Job/Job.h>
#include "../DreamWorldDefine.h"

namespace DreamWorld {
using MS = std::chrono::milliseconds;
class TimerJob
    : public sh::Utility::Job {
 public:
  TimerJob(std::chrono::high_resolution_clock::time_point wakeTime, Job::Caller&& func);

  // priorityQueue에서 우선 순위 판단을 위한 operator
  constexpr bool operator<(const TimerJob& other) const {
    return (m_wakeupTime > other.m_wakeupTime);
  }

  bool IsReady();

  const MS GetRestTimeForReady() const;

  const int64_t GetRestTime() const;

  const int64_t GetWakeTime() const;

 protected:
  std::chrono::high_resolution_clock::time_point m_wakeupTime;
  int64_t m_restTime;
};
}  // namespace DreamWorld
