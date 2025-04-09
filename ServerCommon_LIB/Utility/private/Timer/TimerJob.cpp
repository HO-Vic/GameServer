#include "pch.h"
#include <Timer/TimerJob.h>

namespace sh::Utility {
TimerJob::TimerJob(_chrono_clock::time_point wakeTime, sh::Utility::Job::Caller&& func)
    : sh::Utility::Job(std::move(func)), m_wakeupTime(wakeTime), m_restTime(5) {
}

bool TimerJob::IsReady() {
  m_restTime = GetRestTimeForReady().count();
  return m_restTime <= 3;
}

const MS TimerJob::GetRestTimeForReady() const {
  return std::chrono::duration_cast<MS>(m_wakeupTime - _chrono_clock::now());
}

const int64_t TimerJob::GetRestTime() const {
  return m_restTime;
}

const int64_t TimerJob::GetWakeTime() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(m_wakeupTime.time_since_epoch()).count();
}
}  // namespace sh::Utility