#include "pch.h"
#include "Job.h"
#include "./JobPool/JobPoolBase.h"

namespace sh {
	namespace Utility {
		Job::Job(std::shared_ptr<JobPoolBase> jobPoolRef)
			:m_jobPool(jobPoolRef)
		{
		}
		Job::Job(Caller&& caller, std::shared_ptr<JobPoolBase> jobPoolRef)
			: m_job(std::move(caller)), m_jobPool(jobPoolRef)
		{
		}
		void Job::Set(Caller&& caller, std::shared_ptr<JobPoolBase> jobPoolRef)
		{
			m_job = std::move(caller);
			m_jobPool = jobPoolRef;
		}

		void Job::Set(Caller&& caller)
		{
			BUILD_MESSAGE(__FILE__, __LINE__, "같은 job풀에서 가져온거라면 호출 => shared_ptr::refCnt가 atomic 동작이라 ++ -- 동작은 오버헤드");
			m_job = std::move(caller);
		}

		void Job::ReturnToJobPool()
		{
			auto jobPoolRef = m_jobPool.lock();
			if (nullptr != jobPoolRef) {
				jobPoolRef->PushJob(shared_from_this());
			}
		}
	}
}

