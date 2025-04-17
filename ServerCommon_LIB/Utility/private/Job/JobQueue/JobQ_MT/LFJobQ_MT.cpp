#include <pch.h>
#include <Job/JobQueue/JobQ_MT/LFJobQ_MT.h>
#include "Job/Job.h"

namespace sh {
namespace Utility {

LFJobQ_MT::LFJobQ_MT(const std::string& jobPoolName)
    : JobQBase(jobPoolName) {
}

void LFJobQ_MT::DoJobs(const uint64_t execCnt) {
  size_t executeSize = execCnt;
  if (0 == execCnt) {
    executeSize = m_jobSize;
  }
  uint64_t executeCnt = 0;
  for (int i = 0; i < executeSize; ++i) {
    std::unique_ptr<Job, std::function<void(Job*)>> execJob = nullptr;
    if (!m_jobs.try_pop(execJob)) {
      break;
    }
    execJob->Execute();
    executeCnt++;
  }
  m_jobSize -= executeCnt;
}

void LFJobQ_MT::InsertJob(std::unique_ptr<Job, std::function<void(Job*)>>&& job) {
  m_jobs.push(std::move(job));
  m_jobSize += 1;
}

}
}  // namespace sh::Utility
