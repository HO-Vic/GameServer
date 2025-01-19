#include <pch.h>
#include <Job/Job.h>
#include <BuildMsg/BuildMsg.h>


namespace sh::Utility {

Job::Job()
    : m_job(nullptr) {
}

Job::Job(Caller&& caller)
    : m_job(std::move(caller)) {
}

void Job::Set(Caller&& caller) {
  m_job = std::move(caller);
}

}  // namespace sh::Utility
