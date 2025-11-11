#pragma once
#include <Windows.h>
#include <cstdint>
#include <stop_token>
#include <Utility/Thread/ThreadPool.h>

namespace sh::IO_Engine {
class IO_Core {
 public:
  IO_Core();

  IO_Core(const uint8_t ioThreadNo, const bool metricUse = false);

  void Init();

  void Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize = 1500, const uint32_t sendBufferPoolSize = 1500);

  void Start();

  HANDLE GetHandle() const;

 private:
  Utility::ThreadPool m_threadPool;
};
}  // namespace sh::IO_Engine
