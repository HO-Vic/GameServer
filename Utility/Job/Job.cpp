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

		void Job::Set(Caller&& caller)//같은 풀에서 가져온거라면 굳이? -> shared_ptr::refCnt가 atomic 동작이라 -- ++ 동작 하게 될거 같아서 필요 없으면 ㄴㄴ
		{
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

