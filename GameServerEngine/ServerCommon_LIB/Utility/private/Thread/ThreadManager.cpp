#include <pch.h>
#include <Thread/ThreadManager.h>

namespace sh::Utility {
ThreadManager::ThreadManager(const uint8_t threadNum) {
  m_threads.reserve(threadNum);
}

ThreadManager::~ThreadManager() {
  for (auto& jth : m_threads) {
    jth.join();
  }
}

void ThreadManager::ForceStop() {
  for (auto& jth : m_threads) {
    jth.request_stop();
  }
}

}  // namespace sh::Utility
