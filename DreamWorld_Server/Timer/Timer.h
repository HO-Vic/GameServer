#pragma once
#include <memory>
#include <functional>
#include <tbb/tbb.h>
#include <Utility/SingletonBase/Singleton.h>
#include "TimerJob.h"

namespace DreamWorld {
class Timer
    : public sh::Utility::SingletonBase<Timer> {
  using TimerJobPtr = std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>;
  struct TimerQueueComp {
    constexpr bool operator()(TimerJobPtr& l, TimerJobPtr& r) {
      return *l < *r;
    }
  };

 public:
  ~Timer() = default;

  void Start();

  void InsertTimerEvent(std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>&& timer);

 private:
  void TimerThreadFunc(std::stop_token stopToken);

 private:
  tbb::concurrent_priority_queue<std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>, Timer::TimerQueueComp> m_timerQueue;
};
}  // namespace DreamWorld