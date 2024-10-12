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

		void Job::Set(Caller&& caller)//���� Ǯ���� �����°Ŷ�� ����? -> shared_ptr::refCnt�� atomic �����̶� -- ++ ���� �ϰ� �ɰ� ���Ƽ� �ʿ� ������ ����
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

