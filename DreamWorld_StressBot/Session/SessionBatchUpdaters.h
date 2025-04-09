#pragma once
#include <vector>
#include <memory>
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Thread/ThreadPool.h>
#include <Utility/Job/Job.h>
#include <Utility/Timer/Timer.h>
#include <Utility/Thread/IWorkerItem.h>

namespace Stress {
class SessionBatchUpdater;
class SessionBatchUpdaters
    : public sh::Utility::SingletonBase<SessionBatchUpdaters> {
 public:
  SessionBatchUpdaters() = default;

  ~SessionBatchUpdaters() = default;

  void Init(const uint32_t threadCount, const uint8_t batchUpdaterCnt, const uint8_t timerThreadNo = 1);

  void InsertJobByIdx(const uint32_t idx, sh::Utility::Job::Caller&& caller);

  std::shared_ptr<SessionBatchUpdater>& GetBatchUpdater(const uint32_t);

  void Start() {
    m_threadPool.Start();
  }

  sh::Utility::Timer& GetTimer() {
    return m_timer;
  }

  void InsertUpdaterTimerJob(const sh::Utility::WokerPtr& workerPtr);

 private:
  sh::Utility::ThreadPool m_threadPool;
  sh::Utility::Timer m_timer;
  std::vector<std::shared_ptr<SessionBatchUpdater>> m_batchUpdaters;
};
}  // namespace Stress
