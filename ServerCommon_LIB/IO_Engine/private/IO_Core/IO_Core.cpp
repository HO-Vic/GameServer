#include <pch.h>
#include <IO_Core/IO_Core.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
IO_Core::IO_Core()
    : m_threadPool(1), m_isInit(false) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
#ifdef _DEBUG
    assert(false && "WSAData init fail");
#endif  // _DEBUG
  }
}

IO_Core::IO_Core(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/, const bool metricUse /*= false*/)
    : m_threadPool(ioThreadNo), m_isInit(false) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
#ifdef _DEBUG
    assert(false && "WSAData init fail");
#endif  // _DEBUG
  }
  IO_Metric::GetInstance().Init(metricUse);
  m_threadPool.Init();
  ThWorkerJobPool::GetInstance().Init(thWorkerPoolSize, metricUse);
  SendBufferPool::GetInstance().Init(sendBufferPoolSize, metricUse);
  SendBufferAllocator::GetInstance().Init(sendBufferPoolSize, metricUse);
  m_isInit = true;
}

void IO_Core::Init(const bool metricUse /*= false*/) {
  if (m_isInit) {
    return;
  }
  IO_Metric::GetInstance().Init(metricUse);
  m_threadPool.Init();
  ThWorkerJobPool::GetInstance().Init(1500, metricUse);
  SendBufferPool::GetInstance().Init(1500, metricUse);
  SendBufferAllocator::GetInstance().Init(1500, metricUse);
  m_isInit = true;
}

void IO_Core::Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/, const bool metricUse /* = false*/) {
  if (m_isInit) {
    return;
  }
  IO_Metric::GetInstance().Init(metricUse);
  m_threadPool.Init(ioThreadNo);
  ThWorkerJobPool::GetInstance().Init(thWorkerPoolSize, metricUse);
  SendBufferPool::GetInstance().Init(sendBufferPoolSize, metricUse);
  SendBufferAllocator::GetInstance().Init(sendBufferPoolSize, metricUse);
  m_isInit = true;
}

void IO_Core::Start() {
#ifdef _DEBUG
  assert(true == m_isInit);
#endif  // _DEBUG
  if (false == m_isInit) {
    IO_Core::Init();
  }
  m_threadPool.Start();
}

HANDLE IO_Core::GetHandle() const {
  return m_threadPool.GetHandle();
}

}  // namespace sh::IO_Engine
