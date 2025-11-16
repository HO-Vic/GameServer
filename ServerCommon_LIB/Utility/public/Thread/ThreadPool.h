#pragma once
#include <Windows.h>
// #include "ThreadManager.h"
#include <vector>
#include <thread>

namespace sh::Utility {
class ThreadPool {
 public:
  ThreadPool();

  ~ThreadPool();

  HANDLE GetHandle() const {
    return m_handle;
  }

  void RunningThread() const;

  //if CreateIocp() raised Error then return LastError(), else success then return ERROR_SUCCESS(0)
  int Init(const uint8_t threadNo = 1);

  void Start();

 private:
  HANDLE m_handle;
  // ThreadManager m_threadManager;
  uint8_t m_threadNo;
  std::vector<std::thread> m_threads;
};
}  // namespace sh::Utility
