#pragma once
#include <memory>
#include <string>

namespace sh {
namespace Utility {
class Job;
class JobQBase {
 public:
  static constexpr uint64_t ALL_JOB_EXEC = 0;
  JobQBase(const std::string& name)
      : m_name(name) {
  }

  virtual ~JobQBase() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) = 0;  // 수행할 갯수 지정: 0이면 전부 다 수행

  // 만약에 custom deleter를 사용하지 않는다면, 람다로 delete job을 넘겨야 됨
  virtual void InsertJob(std::unique_ptr<Job, std::function<void(Job*)>>&& job) = 0;

 protected:
  std::string m_name;
};
}
}  // namespace sh::Utility