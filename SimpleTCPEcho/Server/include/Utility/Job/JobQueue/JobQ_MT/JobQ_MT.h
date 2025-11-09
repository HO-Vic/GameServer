#pragma once
#include "../JobQBase.h"
#include <mutex>
// #include "../../../SpinLock/SpinLock.h"

namespace sh {
namespace Utility {

class JobPoolBase;
class JobQ_MT
    : public JobQBase {
 public:
  JobQ_MT() = default;

  JobQ_MT(const std::string& name)
      : JobQBase(name) {
  }

  virtual ~JobQ_MT() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) override;  // 수행할 갯수 지정: 0이면 전부 다 수행

  virtual void InsertJob(std::unique_ptr<Job, std::function<void(Job*)>>&& job) override;

 private:
  std::queue<std::unique_ptr<Job, std::function<void(Job*)>>> m_jobs;
  // std::mutex m_lock;
  std::mutex m_lock;
};
}
}  // namespace sh::Utility