#pragma once
#include "Utility/Job/JobQueue/JobQBase.h"
#include <tbb/concurrent_queue.h>
#include <memory>

namespace sh::Utility {
class Job;
}

namespace DreamWorld {
class LFJobQ : public sh::Utility::JobQBase {
 public:
  LFJobQ() = default;

  LFJobQ(const std::string& jobPoolName);

  virtual ~LFJobQ() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) override;

  virtual void InsertJob(std::unique_ptr<sh::Utility::Job, std::function<void(sh::Utility::Job*)>>&& job) override;

 protected:
 private:
  tbb::concurrent_queue<std::unique_ptr<sh::Utility::Job, std::function<void(sh::Utility::Job*)>>> m_jobs;
  std::atomic_uint64_t m_jobSize;
};
}  // namespace DreamWorld
