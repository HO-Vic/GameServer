#include "pch.h"
#include <cassert>
#include <functional>
#include "Thread/ThreadPool.h"
#include "Thread/ThWorkerJob.h"
#include "BuildMsg/BuildMsg.h"

namespace sh::Utility {
ThreadPool::ThreadPool()
    : m_threadNo(1), m_handle(nullptr) {
}

ThreadPool::~ThreadPool() {
  for (auto& th : m_threads) {
    th.join();
  }
}

void ThreadPool::RunningThread() const {
  static constexpr uint16_t MAX_COMPLETION_CNT = 32;
  OVERLAPPED_ENTRY overlappedEntry[MAX_COMPLETION_CNT];

  while (true) {
    uint32_t getOverlappedCnt = 0;
    bool isSuccess = GetQueuedCompletionStatusEx(m_handle, overlappedEntry, MAX_COMPLETION_CNT, reinterpret_cast<PULONG>(&getOverlappedCnt), INFINITE, false);

    // execute overlapped
    for (uint8_t i = 0; i < getOverlappedCnt; ++i) {
      auto& workerJob = *(reinterpret_cast<ThWorkerJob*>(overlappedEntry[i].lpOverlapped));

      /*if (WORKER_TYPE::TERMINATE == workerJob.GetType()) {
        m_threadManager.ForceStop();
        return;
      }*/

      // Internal은 에러 코드
      // 64비트 자료형이지만, 실제로는 32비트만 사용
      // NTStatus에 대한 오류 코드임
      workerJob(overlappedEntry[i].dwNumberOfBytesTransferred, static_cast<DWORD>(overlappedEntry[i].Internal));
    }
  }
}

int ThreadPool::Init(const uint8_t threadNo /*= 1*/) {
  m_threadNo = threadNo;
  m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_threadNo);
  if (nullptr == m_handle) {
    auto errorNo = GetLastError();
    return static_cast<int>(errorNo);
  }
  return 0;
}

void ThreadPool::Start() {
  for (auto i = 0; i != m_threadNo; ++i) {
    m_threads.emplace_back(std::bind(&ThreadPool::RunningThread, this));
  }
}
}  // namespace sh::Utility
