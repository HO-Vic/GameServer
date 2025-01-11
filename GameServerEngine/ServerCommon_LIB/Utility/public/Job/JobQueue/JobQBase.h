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

			virtual void DoJobs(const uint64_t execCnt = 0) = 0;//수행할 갯수 지정: 0이면 전부 다 수행

			virtual void InsertJob(std::shared_ptr<Job>& job) = 0;

			//jobPool에서 job을 가져오고 인자로 값 변경하고 가져오기, jobPool에 알아서 들어가게 jobPool에서 생성할 때, Job에 넣음, onwerPtr이 nullptr이면 static 함수에 대한 job
			template<typename T, typename ...Args>
			JobPtr GetJob(std::function<void(Args...)> jobFunc, std::shared_ptr<T> ownerPtr = nullptr, Args... args);

		protected:
			std::shared_ptr<JobPoolBase> m_jobPool;
			std::string m_name;
		};

	}
}