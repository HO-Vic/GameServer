#include <Job/Job.h>
#include <Job/JobPool/JobPoolBase.h>
#include <pch.h>

namespace sh::Utility {

template <typename T, typename... Args>
inline JobPtr JobPoolBase::GetJobObject(std::function<void(Args...)> jobFunc,
                                        std::shared_ptr<T> owner,
                                        Args... args) {
  auto job = GetJobObjectByPool();
  job->Set(std::move(Job::GenerateCaller(jobFunc, owner, args)),
           shared_from_this());
  return job;
}
}  // namespace sh::Utility
