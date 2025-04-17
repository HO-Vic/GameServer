#include <pch.h>
#include <Job/Job.h>
#include <Job/JobQueue/JobQ_MT/JobQ_MT.h>

namespace sh {
namespace Utility {

void JobQ_MT::DoJobs(const uint64_t execCnt) {
  static constexpr uint64_t MAX_EXEC_CNT = 100;  // 계속 다른 쓰레드에서 실행할 때 마다 추가하면, 루프가 안 끝남,  추가 end 조건
  uint32_t executeCnt = 0;
  if (ALL_JOB_EXEC == execCnt) {
    while (!m_jobs.empty() && executeCnt != MAX_EXEC_CNT) {
      m_lock.lock();
      std::unique_ptr<Job, std::function<void(Job*)>> job = std::move(m_jobs.front());
      m_jobs.pop();
      m_lock.unlock();
      job->Execute();
      executeCnt++;
    }
  } else {
    auto limitExec = std::min(execCnt, MAX_EXEC_CNT);
    while (!m_jobs.empty() && limitExec != executeCnt) {
      m_lock.lock();
      std::unique_ptr<Job, std::function<void(Job*)>> job = std::move(m_jobs.front());
      m_jobs.pop();
      m_lock.unlock();
      job->Execute();
      executeCnt++;
    }
  }
}

void JobQ_MT::InsertJob(std::unique_ptr<Job, std::function<void(Job*)>>&& job) {
  m_lock.lock();
  m_jobs.push(std::move(job));
  m_lock.unlock();
}
}
}  // namespace sh::Utility
