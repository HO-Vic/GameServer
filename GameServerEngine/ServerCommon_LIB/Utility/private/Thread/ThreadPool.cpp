#include "pch.h"
#include "Thread/ThreadPool.h"
#include <assert.h>
#include "Thread/ThWorkerJob.h"
#include "BuildMsg/BuildMsg.h"

namespace sh::Utility {
ThreadPool::ThreadPool()
    : m_threadManager(1), m_threadNo(1), m_handle(nullptr) {
}

ThreadPool::ThreadPool(const uint8_t threadNo)
    : m_threadManager(threadNo), m_threadNo(threadNo), m_handle(nullptr) {
}

void ThreadPool::RunningThread(std::stop_token stopToken) {
  static constexpr uint16_t MAX_COMPLETION_CNT = 32;
  OVERLAPPED_ENTRY overlappedEntry[MAX_COMPLETION_CNT];

  while (true) {
    uint32_t getOverlappedCnt = 0;
    bool isSuccess = GetQueuedCompletionStatusEx(m_handle, overlappedEntry, MAX_COMPLETION_CNT, reinterpret_cast<PULONG>(&getOverlappedCnt), INFINITE, false);

    // execute overlapped
    for (uint8_t i = 0; i < getOverlappedCnt; ++i) {
      auto& workerJob = *(reinterpret_cast<ThWorkerJob*>(overlappedEntry[i].lpOverlapped));

      if (WORKER_TYPE::TERMINATE == workerJob.GetType()) {
        m_threadManager.ForceStop();
        return;
      }

      workerJob(overlappedEntry[i].dwNumberOfBytesTransferred);
    }
    if (stopToken.stop_requested()) {
      return;
    }
  }
}

void ThreadPool::Init(const uint8_t threadNo) {
  m_threadNo = threadNo;
  Init();
}

void ThreadPool::Init() {
  m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_threadNo);
  if (nullptr == m_handle) {
    auto errorNo = GetLastError();
    assert(nullptr != m_handle);
  }
}

void ThreadPool::Start() {
  for (auto i = 0; i != m_threadNo; ++i) {
    m_threadManager.InsertThread([this](std::stop_token stopToken) {
      this->RunningThread(stopToken);
    });
  }
}
}  // namespace sh::Utility
