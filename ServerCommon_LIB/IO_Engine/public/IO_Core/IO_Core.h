#pragma once
#include <utility>
#include <cstdint>
#include <Windows.h>
#include <stop_token>
#include <Utility/Thread/ThreadPool.h>

namespace sh::IO_Engine {
class IO_Core {
 public:
  enum class ErrorType {
    None = 0,
    Win32Err = 1,    // GetLastError()
    WInsockErr = 2,  // WsaGetLastError()
    IO_Core = 3      // -1: Not Init
  };

 public:
  IO_Core();

  // return threadPool rseult, fail: lastError, success: 0(ERROR_SUCCESS)
  std::pair<ErrorType, int> Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize = 1500, const uint32_t sendBufferPoolSize = 1500, const bool metricUse = false);

  // if	not inited then return -1, else return 0;
  int Start();

  HANDLE GetHandle() const;

 private:
  Utility::ThreadPool m_threadPool{};
  bool m_isInit = false;
};
}  // namespace sh::IO_Engine
