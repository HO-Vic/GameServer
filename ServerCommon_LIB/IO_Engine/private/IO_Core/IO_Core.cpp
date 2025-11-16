#include "pch.h"
#include <IO_Core/IO_Core.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
IO_Core::IO_Core()
    : m_threadPool(), m_isInit(false) {
}

ErrorResult IO_Core::Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/, const bool metricUse /* = false*/) {
  if (m_isInit) {
    return {ErrorType::IO_CORE_NOT_INIT, -1};
  }

  WSADATA wsaData{};
  auto winsockError = WSAStartup(WINSOCK_VERSION, &wsaData);
  if (0 != winsockError) {
    return {ErrorType::WINSOCK_ERR, winsockError};
  }

  auto thResult = m_threadPool.Init(ioThreadNo);
  if (0 != thResult) {
    return {ErrorType::WIN32_ERR, thResult};
  }

  IO_Metric::GetInstance().Init(metricUse);
  ThWorkerJobPool::GetInstance().Init(thWorkerPoolSize, metricUse);
  SendBufferPool::GetInstance().Init(sendBufferPoolSize, metricUse);
  SendBufferAllocator::GetInstance().Init(sendBufferPoolSize, metricUse);
  m_isInit = true;
  return {ErrorType::None, 0};
}

ErrorResult IO_Core::Start() {
#ifdef _DEBUG
  assert(true == m_isInit && "IO_Core not inited");
#endif  // _DEBUG

  if (false == m_isInit) {
    return {ErrorType::IO_CORE_NOT_INIT, -1};
  }
  m_threadPool.Start();
  return {ErrorType::None, 0};
}

HANDLE IO_Core::GetHandle() const {
  return m_threadPool.GetHandle();
}

}  // namespace sh::IO_Engine
