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
    bool operator()(TimerJobPtr& l, TimerJobPtr& r) const {
      return l->GetWakeTime() > r->GetWakeTime();
    }
  };

  using TimerQueue = tbb::concurrent_priority_queue<TimerJobPtr, Timer::TimerQueueComp>;

 public:
  ~Timer() = default;

  void Start(const uint8_t threadNo = 1);

  void InsertTimerEvent(std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>&& timer);

 private:
  void TimerThreadFunc(std::stop_token stopToken, uint8_t thId);

  void TimerThreadFunc2(std::stop_token stopToken, uint8_t thId);

  std::vector<std::unique_ptr<TimerQueue>> m_timerQueues;
  std::atomic<uint8_t> m_loadBalancer = 0;
  uint8_t m_threadNo = 1;
};
}  // namespace DreamWorld