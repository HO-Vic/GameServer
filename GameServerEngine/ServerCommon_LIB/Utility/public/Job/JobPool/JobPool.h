#pragma once
#include <Pool/ObjectPool.h>

namespace sh::Utility {
class Job;
using JobPtr = std::shared_ptr<Job>;
class JobPool : public ObjectPool<JobPtr>, public std::enable_shared_from_this<JobPool> {
 public:
  JobPool(std::string name, const uint32_t defaultPoolSize)
      : m_name(name), ObjectPool<JobPtr>(defaultPoolSize) {
  }
  virtual ~JobPool() {
  }

  template <typename T, typename... Args>
  JobPtr GetJobObject(std::function<void(Args...)> jobFunc, std::shared_ptr<T>& owner, Args... args);

 protected:
  std::string m_name;
};
}  // namespace sh::Utility
