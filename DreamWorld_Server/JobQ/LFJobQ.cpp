#include <stdafx.h>
#include "LFJobQ.h"
#include "Utility/Job/Job.h"

namespace DreamWorld {

LFJobQ::LFJobQ(const std::string& jobPoolName)
    : JobQBase(jobPoolName) {
}

void LFJobQ::DoJobs(const uint64_t execCnt) {
  size_t executeSize = execCnt;
  if (0 == execCnt) {
    executeSize = m_jobSize;
  }
  uint64_t executeCnt = 0;
  for (int i = 0; i < executeSize; ++i) {
    std::unique_ptr<sh::Utility::Job, std::function<void(sh::Utility::Job*)>> execJob = nullptr;
    if (!m_jobs.try_pop(execJob)) {
      break;
    }
    execJob->Execute();
    executeCnt++;
  }
  m_jobSize -= executeCnt;
}

void LFJobQ::InsertJob(std::unique_ptr<sh::Utility::Job, std::function<void(sh::Utility::Job*)>>&& job) {
  m_jobs.push(std::move(job));
  m_jobSize += 1;
}

}  // namespace DreamWorld
