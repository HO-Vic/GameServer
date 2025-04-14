#include <pch.h>
#include <IO_Core/IO_Core.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
IO_Core::IO_Core()
    : m_threadPool(1) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
    assert("WSAData init fail");
  }
  IO_MetricSlot::GetInstance().Init();
}

IO_Core::IO_Core(const uint8_t ioThreadNo, const bool metricUse /*= false*/)
    : m_threadPool(ioThreadNo) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
    assert("WSAData init fail");
  }
  IO_MetricSlot::GetInstance().Init(metricUse);
}

void IO_Core::Init() {
  m_threadPool.Init();
}

void IO_Core::Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/) {
  m_threadPool.Init(ioThreadNo);
  ThWorkerJobPool::GetInstance().Init(thWorkerPoolSize);
  SendBufferPool::GetInstance().InitSize(sendBufferPoolSize);
}

void IO_Core::Start() {
  m_threadPool.Start();
}

HANDLE IO_Core::GetHandle() const {
  return m_threadPool.GetHandle();
}

}  // namespace sh::IO_Engine
