#pragma once
#include <pch.h>
#include <Job/JobQueue/JobQBase.h>

/*
        �Լ��ڿ� ���ؼ� queue�� �����ߴٰ�
        �ѹ��� ����
        tbb::concurrent_queue�� Ȱ���Ͽ� lf�ϰ�
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
