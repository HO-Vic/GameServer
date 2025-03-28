#include "stdafx.h"
#include "TimerJob.h"

namespace DreamWorld {
TimerJob::TimerJob(std::chrono::high_resolution_clock::time_point wakeTime, sh::Utility::Job::Caller&& func)
    : sh::Utility::Job(std::move(func)), m_wakeupTime(wakeTime) {
}

bool TimerJob::IsReady() const {
  auto laterTime = GetRestTimeForReady().count();
  if (laterTime > 3) {
    return false;
  }
  /*if (laterTime < -19) {
          spdlog::warn("Timer LaterTime: {}", laterTime);
          spdlog::critical("laterTime > 19");
  }*/
  return true;
  // return m_wakeupTime <= std::chrono::high_resolution_clock::now();
  return laterTime <= 0;
}

const DreamWorld::MS TimerJob::GetRestTimeForReady() const {
  return std::chrono::duration_cast<DreamWorld::MS>(m_wakeupTime - std::chrono::high_resolution_clock::now());
}
}  // namespace DreamWorld