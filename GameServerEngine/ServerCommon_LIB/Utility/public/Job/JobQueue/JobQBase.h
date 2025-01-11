#pragma once
#include <pch.h>

namespace sh {
	namespace Utility {
		class Job;
		class JobPoolBase;
		class JobQBase
		{
		public:
			static constexpr uint64_t ALL_JOB_EXEC = 0;
			using JobPtr = std::shared_ptr<Job>;

			JobQBase(const std::string& name)
				:m_name(name), m_jobPool(nullptr)
			{}

			JobQBase(const std::string& name, std::shared_ptr<JobPoolBase> jobPool)
				:m_name(name), m_jobPool(jobPool)
			{}

			virtual ~JobQBase() {}

			virtual void DoJobs(const uint64_t execCnt = 0) = 0;//������ ���� ����: 0�̸� ���� �� ����

			virtual void InsertJob(std::shared_ptr<Job>& job) = 0;

			//jobPool���� job�� �������� ���ڷ� �� �����ϰ� ��������, jobPool�� �˾Ƽ� ���� jobPool���� ������ ��, Job�� ����, onwerPtr�� nullptr�̸� static �Լ��� ���� job
			template<typename T, typename ...Args>
			JobPtr GetJob(std::function<void(Args...)> jobFunc, std::shared_ptr<T> ownerPtr = nullptr, Args... args);

		protected:
			std::shared_ptr<JobPoolBase> m_jobPool;
			std::string m_name;
		};

	}
}