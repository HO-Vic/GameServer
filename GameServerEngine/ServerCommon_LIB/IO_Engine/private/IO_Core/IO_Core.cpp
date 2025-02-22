#include <pch.h>
#include <IO_Core/IO_Core.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>

namespace sh::IO_Engine {
IO_Core::IO_Core(const uint16_t ioThreadNo)
    : m_threadPool(ioThreadNo), m_ioThreadNo(ioThreadNo), m_iocpHandle(nullptr) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
    assert("WSAData init fail");
  }
}

void IO_Core::Init() {
  m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_ioThreadNo);
  if (nullptr == m_iocpHandle) {
    auto errorNo = GetLastError();
    assert(nullptr != m_iocpHandle);
  }
}

void IO_Core::Start() {
  for (uint32_t i = 0; i < m_ioThreadNo; ++i) {
    m_threadPool.InsertThread(
        [this](std::stop_token stopToken, const uint32_t threadNo) {
          this->IO_Thread(stopToken, threadNo);
        },
        i + 1);
  }
}

HANDLE IO_Core::GetHandle() const {
  return m_iocpHandle;
}

void IO_Core::IO_Thread(std::stop_token& stopToken, const uint32_t ThreadNo) {
  static constexpr uint16_t MAX_COMPLETION_CNT = 10;
  OVERLAPPED_ENTRY overlappedEntry[MAX_COMPLETION_CNT];

  while (true) {
    uint32_t getOverlappedCnt = 0;
    bool isSuccess = GetQueuedCompletionStatusEx(m_iocpHandle, overlappedEntry, MAX_COMPLETION_CNT, reinterpret_cast<PULONG>(&getOverlappedCnt), INFINITE, false);

    // execute overlapped
    for (uint8_t i = 0; i < getOverlappedCnt; ++i) {
      auto& overalppedEx = *(reinterpret_cast<OverlappedEx*>(overlappedEntry[i].lpOverlapped));

      if (OVERLAPPED_EVENT_TYPE::TERMINATE == overalppedEx.m_type) {
        BUILD_MESSAGE(__FILE__, __LINE__, "TERMINATE가 호출 됐을 때, stopToken으로 모든 IO Thread 종료 기대");
        m_threadPool.ForceStop();
        return;
      }

      overalppedEx(overlappedEntry[i].dwNumberOfBytesTransferred);
    }
    if (stopToken.stop_requested()) {
      return;
    }
  }
}
}  // namespace sh::IO_Engine
