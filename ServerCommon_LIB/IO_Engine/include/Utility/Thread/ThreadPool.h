#pragma once
#include <Windows.h>
// #include "ThreadManager.h"
#include <vector>
#include <thread>

namespace sh::Utility {
class ThreadPool {
 public:
  ThreadPool();

  ThreadPool(const uint8_t threadNo);

  ~ThreadPool();

  HANDLE GetHandle() const {
    return m_handle;
  }

  void RunningThread() const;

  void Init(const uint8_t threadNo);

  void Init();

  void Start();

 private:
  HANDLE m_handle;
  // ThreadManager m_threadManager;
  uint8_t m_threadNo;
  std::vector<std::thread> m_threads;
};
}  // namespace sh::Utility
