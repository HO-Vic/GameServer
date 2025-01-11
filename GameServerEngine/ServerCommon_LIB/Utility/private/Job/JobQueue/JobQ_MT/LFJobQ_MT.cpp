#include <pch.h>
#include <Job/JobQueue/JobQ_MT/LFJobQ_MT.h>
#include "Job/Job.h"

namespace sh {
	namespace Utility {

		LFJobQ_MT::LFJobQ_MT(const std::string& jobPoolName, std::shared_ptr<JobPoolBase> jobPool)
			: JobQBase(jobPoolName, jobPool)
		{
		}

		void LFJobQ_MT::DoJobs(const uint64_t execCnt)
		{
			size_t executeSize = execCnt;
			if (0 == execCnt) {
				executeSize = m_jobSize;
			}
			for (int i = 0; i < executeSize; ++i) {
				std::shared_ptr<Job> execJob = nullptr;
				m_jobs.try_pop(execJob);
				execJob->Execute();
				execJob->ReturnToJobPool();
			}
			m_jobSize -= executeSize;
		}

		void LFJobQ_MT::InsertJob(std::shared_ptr<Job>& job)
		{
			m_jobs.push(job);
			m_jobSize += 1;
		}

	}
}
