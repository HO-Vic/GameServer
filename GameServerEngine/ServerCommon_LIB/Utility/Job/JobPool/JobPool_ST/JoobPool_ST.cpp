#include "pch.h"
#include "JoobPool_ST.h"
#include "../../Job.h"

namespace sh {
	namespace Utility {
		JoobPool_ST::JoobPool_ST(std::string& name, const uint32_t poolSize)
			: JobPoolBase(name)
		{
			for (uint32_t i = 0; i < poolSize; ++i) {
				m_jobs.push(std::make_shared<Job>(shared_from_this()));
			}
		}

		void JoobPool_ST::PushJob(JobPtr jobPtr)
		{
			m_jobs.push(jobPtr);
		}

		JobPtr JoobPool_ST::GetJobObjectByPool()
		{
			auto jobPtr = m_jobs.front();
			m_jobs.pop();
			return jobPtr;
		}


	}
}
