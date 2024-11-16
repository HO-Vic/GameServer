#include "pch.h"
#include "JobPoolBase.h"
#include "../Job.h"

namespace sh {
	namespace Utility {
		template<typename T, typename ...Args>
		inline JobPtr JobPoolBase::GetJobObject(std::function<void(Args...)> jobFunc, std::shared_ptr<T> owner, Args ...args)
		{
			auto job = GetJobObjectByPool();
			job->Set(std::move(Job::GenerateCaller(jobFunc, owner, args)), shared_from_this());
			return job;
		}


	}
}

