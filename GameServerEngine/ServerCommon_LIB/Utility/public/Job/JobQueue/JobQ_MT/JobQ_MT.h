#pragma once
#include <pch.h>
#include <Job/JobQueue/JobQBase.h>
#include <SpinLock/SpinLock.h>

namespace sh {
namespace Utility {

class JobPoolBase;
class JobQ_MT
    : public JobQBase {
 public:
  JobQ_MT(const std::string& name)
      : JobQBase(name) {
  }

  virtual ~JobQ_MT() {
  }

  virtual void DoJobs(const uint64_t execCnt = 0) override;  // ������ ���� ����: 0�̸� ���� �� ����

  virtual void InsertJob(std::shared_ptr<Job>& job) override;

 private:
  std::queue<std::shared_ptr<Job>> m_jobs;
  // std::mutex m_lock;
  SpinLock m_lock;
};
}
}  // namespace sh::Utility