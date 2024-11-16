#include "pch.h"
#include "JobQBase.h"
#include "../JobPool/JobPoolBase.h"
#include "../Job.h"

namespace sh {
	namespace Utility {
		template<typename T, typename ...Args>
		inline JobQBase::JobPtr JobQBase::GetJob(std::function<void(Args...)> jobFunc, std::shared_ptr<T> ownerPtr/*=nullptr*/, Args ...args)
		{
			if (nullptr == m_jobPool) {//풀이 없는 잡큐면, 새로 생성한다
				return std::make_shared<Job>(std::move(Job::GenerateCaller(jobFunc, ownerPtr, args)));
			}
			auto jobPtr = m_jobPool->GetJobObject(jobFunc, ownerPtr, args...);
			return jobPtr;
		}
	}
}
