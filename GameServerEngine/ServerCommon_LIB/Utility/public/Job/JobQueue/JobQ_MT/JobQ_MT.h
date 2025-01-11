#pragma once
#include <pch.h>
#include <Job/JobQueue/JobQBase.h>
#include <SpinLock/SpinLock.h>

namespace sh {
	namespace Utility {

		class JobPoolBase;
		class JobQ_MT
			: public JobQBase
		{
		public:
			JobQ_MT(const std::string& name, std::shared_ptr<JobPoolBase> jobPool = nullptr)
				: JobQBase(name, jobPool) {}

			virtual ~JobQ_MT() {}

			virtual void DoJobs(const uint64_t execCnt = 0) override;//수행할 갯수 지정: 0이면 전부 다 수행

			virtual void InsertJob(std::shared_ptr<Job>& job) override;

		private:
			std::queue<std::shared_ptr<Job>> m_jobs;
			//std::mutex m_lock;
			SpinLock m_lock;
		};
	}
}