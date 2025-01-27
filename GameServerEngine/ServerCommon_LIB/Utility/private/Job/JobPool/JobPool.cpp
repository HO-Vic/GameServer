#include <pch.h>
#include <Job/JobPool/JobPool.h>
#include <Job/Job.h>

namespace sh::Utility {

template <typename T, typename... Args>
JobPtr JobPool::GetJobObject(std::function<void(Args...)> jobFunc, std::shared_ptr<T>& owner, Args... args) {
  return ObjectPool<JobPtr>::MakeShared(std::move(Job::GenerateCaller(jobFunc, owner, std::forward<Args>(args)...)));
}
}  // namespace sh::Utility
