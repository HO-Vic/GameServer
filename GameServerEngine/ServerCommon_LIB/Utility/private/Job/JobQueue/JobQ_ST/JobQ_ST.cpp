#include <pch.h>
#include <Job/JobQueue/JobQ_ST/JobQ_ST.h>
#include <Job/Job.h>

namespace sh {
	namespace Utility {
		void JobQ_ST::DoJobs(const uint64_t execCnt)
		{
			if (ALL_JOB_EXEC == execCnt) {
				while (!m_jobs.empty()) {
					JobPtr job = m_jobs.front();
					m_jobs.pop();
					job->Execute();
				}
			}
			else {
				uint32_t executeCnt = 0;
				while (!m_jobs.empty() && execCnt != executeCnt) {
					JobPtr job = m_jobs.front();
					m_jobs.pop();
					job->Execute();
					executeCnt++;
				}
			}
		}

		void JobQ_ST::InsertJob(std::shared_ptr<Job>& job)
		{
			m_jobs.push(job);
		}
	}
}
