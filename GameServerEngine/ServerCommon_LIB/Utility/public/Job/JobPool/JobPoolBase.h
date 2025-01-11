#pragma once
#include <pch.h>

namespace sh::Utility {
class Job;
using JobPtr = std::shared_ptr<Job>;
class JobPoolBase : public std::enable_shared_from_this<JobPoolBase> {
 public:
  JobPoolBase(std::string name) : m_name(name) {}
  virtual ~JobPoolBase() {}

  virtual void PushJob(JobPtr jobPtr) = 0;

  template <typename T, typename... Args>
  JobPtr GetJobObject(std::function<void(Args...)> jobFunc,
                      std::shared_ptr<T> owner = nullptr, Args... args);

 protected:
  virtual JobPtr GetJobObjectByPool() = 0;

 protected:
  std::string m_name;
};
}  // namespace sh::Utility
