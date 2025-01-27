#include "pch.h"
#include <IO_Core/IO_Core.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>

namespace sh::IO_Engine {
IO_Core::IO_Core(const uint16_t ioThreadNo)
    : m_threadPool(ioThreadNo), m_ioThreadNo(ioThreadNo), m_iocpHandle(nullptr) {
}

void IO_Core::Init() {
  m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_ioThreadNo);
  if (nullptr == m_iocpHandle) {
    auto errorNo = GetLastError();
    assert(nullptr != m_iocpHandle);
  }
}

void IO_Core::IO_Thread(std::stop_token& stopToken, const uint32_t ThreadNo) {
  static constexpr uint16_t MAX_COMPLETION_CNT = 10;
  OVERLAPPED_ENTRY overlappedEntry[MAX_COMPLETION_CNT];

  while (true) {
    uint32_t getOverlappedCnt = 0;
    bool isSuccess = GetQueuedCompletionStatusEx(m_iocpHandle, overlappedEntry, MAX_COMPLETION_CNT, reinterpret_cast<PULONG>(&getOverlappedCnt), INFINITE, false);

    // execute overlapped
    for (uint8_t i = 0; i < getOverlappedCnt; ++i) {
      auto& overalppedEx = (*reinterpret_cast<OverlappedEx*>(&overlappedEntry[i].lpOverlapped));
      overalppedEx(overlappedEntry[i].dwNumberOfBytesTransferred);

      if (OVERLAPPED_EVENT_TYPE::TERMINATE == overalppedEx.m_type) {
        m_threadPool.ForceStop();
        return;
      }
    }
    if (stopToken.stop_requested()) {
      return;
    }
  }
}
}  // namespace sh::IO_Engine
