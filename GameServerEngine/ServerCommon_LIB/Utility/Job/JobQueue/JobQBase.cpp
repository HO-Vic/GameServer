#include "pch.h"
#include "JobQBase.h"
#include "../JobPool/JobPoolBase.h"
#include "../Job.h"

namespace sh {
	namespace Utility {
		template<typename T, typename ...Args>
		inline JobQBase::JobPtr JobQBase::GetJob(std::function<void(Args...)> jobFunc, std::shared_ptr<T> ownerPtr/*=nullptr*/, Args ...args)
		{
			if (nullptr == m_jobPool) {//Ǯ�� ���� ��ť��, ���� �����Ѵ�
				return std::make_shared<Job>(std::move(Job::GenerateCaller(jobFunc, ownerPtr, args)));
			}
			auto jobPtr = m_jobPool->GetJobObject(jobFunc, ownerPtr, args...);
			return jobPtr;
		}
	}
}
