#pragma once
#include <Job/JobPool/JobPoolBase.h>

namespace sh::Utility {

class JoobPool_ST : public JobPoolBase {
 public:
  JoobPool_ST(std::string& name, const uint32_t poolSize);

  virtual ~JoobPool_ST() {
  }

  virtual void PushJob(JobPtr jobPtr) override;

 protected:
  virtual JobPtr GetJobObjectByPool() override;

 private:
  std::queue<JobPtr> m_jobs;
};
}  // namespace sh::Utility
