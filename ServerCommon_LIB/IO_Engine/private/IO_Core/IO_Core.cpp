#include <pch.h>
#include <IO_Core/IO_Core.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
IO_Core::IO_Core()
    : m_threadPool(), m_isInit(false) {
}

std::pair<IO_Core::ErrorType, int> IO_Core::Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/, const bool metricUse /* = false*/) {
  if (m_isInit) {
    return {ErrorType::IO_Core, -1};
  }

  WSADATA wsaData{};
  auto winsockError = WSAStartup(WINSOCK_VERSION, &wsaData);
  if (0 != winsockError) {
    return {ErrorType::WInsockErr, winsockError};
  }

  auto thResult = m_threadPool.Init(ioThreadNo);
  if (0 != thResult) {
    return {ErrorType::Win32Err, thResult};
  }

  IO_Metric::GetInstance().Init(metricUse);
  ThWorkerJobPool::GetInstance().Init(thWorkerPoolSize, metricUse);
  SendBufferPool::GetInstance().Init(sendBufferPoolSize, metricUse);
  SendBufferAllocator::GetInstance().Init(sendBufferPoolSize, metricUse);
  m_isInit = true;
  return {ErrorType::None, 0};
}

int IO_Core::Start() {
  if (false == m_isInit) {
    return -1;
  }
  m_threadPool.Start();
  return 0;
}

HANDLE IO_Core::GetHandle() const {
  return m_threadPool.GetHandle();
}

}  // namespace sh::IO_Engine
