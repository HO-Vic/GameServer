#pragma once
#include <pch.h>

namespace sh {
namespace Utility {
class Job;
class JobPoolBase;
class JobQBase {
 public:
  static constexpr uint64_t ALL_JOB_EXEC = 0;
  using JobPtr = std::shared_ptr<Job>;

  JobQBase(const std::string& name)
      : m_name(name) {
  }

  JobQBase(const std::string& name, std::shared_ptr<JobPoolBase> jobPool)
      : m_name(name) {
  }

  virtual ~JobQBase() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) = 0;  // 수행할 갯수 지정: 0이면 전부 다 수행

  virtual void InsertJob(std::shared_ptr<Job>& job) = 0;

 protected:
  std::string m_name;
};
}
}  // namespace sh::Utility