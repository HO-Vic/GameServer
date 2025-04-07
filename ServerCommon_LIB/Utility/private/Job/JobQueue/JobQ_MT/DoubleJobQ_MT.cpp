#include "pch.h"
#include "pch.h"
#include <Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>
#include <Job/Job.h>

namespace sh::Utility {
void DoubleJobQ_MT::DoJobs(const uint64_t execCnt) {
  {
    std::lock_guard<std::mutex> lg{m_lock};
    m_insertIdx = !m_insertIdx;
  }
  auto& taskQueue = m_jobQs[!m_insertIdx];
  while (!taskQueue.empty()) {
    taskQueue.front().get()->Execute();
    taskQueue.pop();
  }
}

void DoubleJobQ_MT::InsertJob(JobPtr&& job) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_jobQs[m_insertIdx].push(std::move(job));
}

}  // namespace sh::Utility