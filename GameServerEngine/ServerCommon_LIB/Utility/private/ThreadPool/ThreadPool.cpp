#include <pch.h>
#include <ThreadPool/ThreadPool.h>

namespace sh::Utility {

ThreadPool::ThreadPool(const int threadNum) {
  m_threads.reserve(threadNum);
}

ThreadPool::~ThreadPool() {
  for (auto& jth : m_threads) {
    jth.join();
  }
}

void ThreadPool::ForceStop() {
  for (auto& jth : m_threads) {
    jth.request_stop();
  }
}

}  // namespace sh::Utility
