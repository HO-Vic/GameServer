#include "pch.h"
#include "LF_JobPool_MT.h"
#include "../../Job.h"
#include "../../../Log/LogManager.h"

namespace sh {
	namespace Utility {

		LF_JobPool_MT::LF_JobPool_MT(const std::string& name, const uint32_t poolSize)
			: JobPoolBase(name), m_newJobCnt(0)
		{
			for (uint32_t i = 0; i < poolSize; ++i) {
				m_jobs.push(std::make_shared<Job>(shared_from_this()));
			}
		}

		void LF_JobPool_MT::PushJob(JobPtr jobPtr)
		{
			m_jobs.push(jobPtr);
		}

		JobPtr LF_JobPool_MT::GetJobObjectByPool()
		{
			JobPtr returnJob;
			bool isSuccess = m_jobs.try_pop(returnJob);
			if (!isSuccess) {//풀에 없다면 새로 생성
#ifdef _DEBUG
				WRITE_LOG(spdlog::level::info, __FUNCTION__, __LINE__, "%s(),- line.%d, JobPoolName: %s, newPoolCnt: %d", m_name, ++m_newJobCnt);
#endif // _DEBUG
				returnJob = std::make_shared<Job>(shared_from_this());
				return returnJob;
			}
			return returnJob;
		}
	}
}
