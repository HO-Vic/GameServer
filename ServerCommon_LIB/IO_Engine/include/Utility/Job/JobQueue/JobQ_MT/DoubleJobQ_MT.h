#pragma once
#include "../JobQBase.h"
#include <queue>
#include <string>
#include <functional>
#include <mutex>

namespace sh::Utility {
class DoubleJobQ_MT
    : public JobQBase {
  using JobPtr = std::unique_ptr<Job, std::function<void(Job*)>>;

 public:
  DoubleJobQ_MT(const std::string& name)
      : JobQBase(name) {
  }

  virtual ~DoubleJobQ_MT() = default;

  virtual void DoJobs(const uint64_t execCnt = 0) override;  // 수행할 갯수 지정: 0이면 전부 다 수행

  // 만약에 custom deleter를 사용하지 않는다면, 람다로 delete job을 넘겨야 됨
  virtual void InsertJob(JobPtr&& job) override;

 private:
  std::mutex m_lock;
  std::queue<JobPtr> m_jobQs[2];
  bool m_insertIdx = false;
};
}  // namespace sh::Utility
