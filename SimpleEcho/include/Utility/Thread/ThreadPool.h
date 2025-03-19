#pragma once
#include <Windows.h>
#include "ThreadManager.h"

namespace sh::Utility {
class ThreadPool {
 public:
  ThreadPool();

  ThreadPool(const uint8_t threadNo);

  HANDLE GetHandle() const {
    return m_handle;
  }

  void RunningThread(std::stop_token stopToken);

  void Init();

  void Start();

 private:
  HANDLE m_handle;
  ThreadManager m_threadManager;
  uint8_t m_threadNo;
};
}  // namespace sh::Utility
