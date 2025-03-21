#pragma once
#include "../JobQBase.h"
#include <tbb/tbb.h>

/*
        함수자에 대해서 queue에 저장했다가
        한번에 수행
        tbb::concurrent_queue를 활용하여 lf하게
*/

namespace sh {
namespace Utility {
class Job;
class JobPoolBase;
class LFJobQ_MT : public JobQBase {
 public:
  LFJobQ_MT(const std::string& jobPoolName);

  virtual ~LFJobQ_MT() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) override;

  virtual void InsertJob(std::shared_ptr<Job>& job) override;

 protected:
 private:
  tbb::concurrent_queue<std::shared_ptr<Job>> m_jobs;
  std::atomic_uint64_t m_jobSize;
};
}
}  // namespace sh::Utility
