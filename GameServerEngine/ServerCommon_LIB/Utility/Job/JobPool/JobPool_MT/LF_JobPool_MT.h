#pragma once
#include "../../../pch.h"
#include "../JobPoolBase.h"
namespace sh {
	namespace Utility {

		class Job;
		class LF_JobPool_MT
			: public JobPoolBase
		{
		public:
			LF_JobPool_MT(const std::string& name, const uint32_t poolSize);

			virtual ~LF_JobPool_MT() {}

			virtual void PushJob(JobPtr jobPtr) override;

		protected:
			virtual JobPtr GetJobObjectByPool() override;

		private:
			tbb::concurrent_queue<std::shared_ptr<Job>> m_jobs;
			std::string m_name;//식별 할 수 있는 이름
#ifdef _DEBUG
			std::atomic_int m_newJobCnt;
#endif // _DEBUG

		};
	}
}


