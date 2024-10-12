#include "pch.h"
#include "JoobPool_MT.h"
#include "../Job/Job.h"

namespace sh {
	namespace Utility {
		JoobPool_MT::JoobPool_MT(std::string& name, const uint32_t poolSize)
			: JobPoolBase(name)
		{
			std::lock_guard<std::mutex> lg(m_lock);
			for (int i = 0; i < poolSize; ++i) {
				m_jobs.push(std::make_shared<Job>(shared_from_this()));
			}
		}

		void JoobPool_MT::PushJob(JobPtr jobPtr)
		{
			std::lock_guard<std::mutex> lg(m_lock);
			m_jobs.push(jobPtr);
		}

		JobPtr JoobPool_MT::GetJobObjectByPool()
		{
			std::lock_guard<std::mutex> lg(m_lock);
			auto jobPtr = m_jobs.front();
			m_jobs.pop();
			return jobPtr;
		}

	}
}

