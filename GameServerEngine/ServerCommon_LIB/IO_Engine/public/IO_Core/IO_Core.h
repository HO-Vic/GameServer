#pragma once
#include <pch.h>
#include <Utility/ThreadPool/ThreadPool.h>

namespace sh::IO_Engine {
class IO_Core {
 public:
  IO_Core(const uint16_t ioThreadNo);

  void Init();

  void IO_Thread(std::stop_token& stopToken, const uint32_t ThreadNo);

 private:
  Utility::ThreadPool m_threadPool;
  HANDLE m_iocpHandle;
  volatile bool m_isRunAble = true;
  uint16_t m_ioThreadNo;
};
}  // namespace sh::IO_Engine
