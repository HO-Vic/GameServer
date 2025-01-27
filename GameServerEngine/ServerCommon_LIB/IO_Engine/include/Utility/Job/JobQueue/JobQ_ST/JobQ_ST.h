#pragma once
#include <pch.h>
#include <Job/JobQueue/JobQBase.h>

namespace sh {
namespace Utility {
class Job;
class JobPoolBase;
class JobQ_ST : public JobQBase {
 public:
  JobQ_ST(const std::string& name)
      : JobQBase(name) {
  }

  virtual ~JobQ_ST() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) override;  // 수행할 갯수 지정: 0이면 전부 다 수행

  virtual void InsertJob(std::shared_ptr<Job>& job) override;

 private:
  std::queue<std::shared_ptr<Job>> m_jobs;
};
}
}  // namespace sh::Utility
