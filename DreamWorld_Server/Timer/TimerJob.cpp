#include "stdafx.h"
#include "TimerJob.h"
#include "../DreamWorldDefine.h"
#include "../LogManager/LogManager.h"

namespace DreamWorld {
TimerJob::TimerJob(std::chrono::high_resolution_clock::time_point wakeTime, sh::Utility::Job::Caller&& func)
    : sh::Utility::Job(std::move(func)), m_wakeupTime(wakeTime) {
}

bool TimerJob::IsReady() {
  m_restTime = GetRestTimeForReady().count();
  return m_restTime <= 3;
}

const DreamWorld::MS TimerJob::GetRestTimeForReady() const {
  return std::chrono::duration_cast<DreamWorld::MS>(m_wakeupTime - chrono_clock::now());
}

const int64_t TimerJob::GetRestTime() const {
  return m_restTime;
}

const int64_t TimerJob::GetWakeTime() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(m_wakeupTime.time_since_epoch()).count();
}
}  // namespace DreamWorld