#pragma once
#include "../JobPoolBase.h"
namespace sh {
	namespace Utility {

		class Job;
		class JoobPool_MT
			: public JobPoolBase
		{
		public:
			JoobPool_MT(std::string& name, const uint32_t poolSize);

			virtual ~JoobPool_MT() {}

			virtual void PushJob(JobPtr jobPtr) override;

		protected:
			virtual JobPtr GetJobObjectByPool() override;

		private:
			std::mutex m_lock;
			std::queue<JobPtr> m_jobs;
		};

	}
}