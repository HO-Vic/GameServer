#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <stop_token>
#include <tbb/concurrent_priority_queue.h>
#include "../Thread/ThreadManager.h"
#include "TimerJob.h"

namespace sh::Utility {
static auto TimerDefaultDeleter = [](TimerJob* ptr) { delete ptr; };
using TimerJobPtr = std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>;
class Timer {
  struct TimerQueueComp {
    bool operator()(TimerJobPtr& l, TimerJobPtr& r) const;
  };

  using TimerQueue = tbb::concurrent_priority_queue<TimerJobPtr, Timer::TimerQueueComp>;

 public:
  Timer() = default;

  ~Timer();

  void Start(const uint8_t threadNo = 1);

  void InsertTimerEvent(TimerJobPtr&& timer);

 private:
  void TimerThreadFunc(std::stop_token stopToken, uint8_t thId);

  void TimerThreadFunc2(std::stop_token stopToken, uint8_t thId);

  std::vector<std::unique_ptr<TimerQueue>> m_timerQueues;
  ThreadManager m_threadManager;
  uint8_t m_threadNo = 1;
};
}  // namespace sh::Utility