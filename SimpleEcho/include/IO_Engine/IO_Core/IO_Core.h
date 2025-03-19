#pragma once
#include <Windows.h>
#include <cstdint>
#include <stop_token>
#include <Utility/Thread/ThreadPool.h>

namespace sh::IO_Engine {
class IO_Core {
 public:
  IO_Core(const uint8_t ioThreadNo);

  void Init();

  void Start();

  HANDLE GetHandle() const;

 private:
  Utility::ThreadPool m_threadPool;
};
}  // namespace sh::IO_Engine
