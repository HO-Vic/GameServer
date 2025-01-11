#pragma once
#include <pch.h>
#include <Job/JobQueue/JobQBase.h>
//#include "../../ThreadPool/ThreadPool.h"

namespace sh {
	namespace Utility {
		class Job;
		class JobPoolBase;
		class JobQ_ST : public JobQBase
		{
		public:
			JobQ_ST(const std::string& name)
				: JobQBase(name)
			{}
			JobQ_ST(const std::string& name, std::shared_ptr<JobPoolBase> jobPool)
				: JobQBase(name, jobPool)
			{}
			virtual ~JobQ_ST() {}

			virtual void DoJobs(const uint64_t execCnt = 0) override;//������ ���� ����: 0�̸� ���� �� ����

			virtual void InsertJob(std::shared_ptr<Job>& job) override;

		private:
			std::queue<std::shared_ptr<Job>> m_jobs;
		};
	}
}
