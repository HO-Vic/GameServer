#include "pch.h"
#include <IO_Metric/IO_Metric.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
void IO_Metric::Init(bool isUse) {
  m_isUse = isUse;
}

IO_MetricSlot& IO_Metric::SwapAndLoad() {
  if (!m_isUse) {
    return m_metics[0];
  }

  // Swap하기전에 오브젝트 풀 사용량 기록을 먼저
  auto taskIndex = m_index.load();
  {
    ThWorkerJobPool::GetInstance().RecordMetric(m_metics[taskIndex]);

    SendBufferAllocator::GetInstance().RecordMertric(m_metics[taskIndex]);
  }
  auto otherIndex = (taskIndex + 1) % 2;
  m_metics[otherIndex].Reset();
  m_index.store(otherIndex);

  return m_metics[taskIndex];
}

void IO_Metric::RecordSend(const DWORD ioByte) {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].sendCompletion.fetch_add(1);
  m_metics[taskIndex].sendByte.fetch_add(ioByte);
}

void IO_Metric::RecordRecv(const DWORD ioByte) {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].recvCompletion.fetch_add(1);
  m_metics[taskIndex].recvByte.fetch_add(ioByte);
}

void IO_Metric::RecordDynamicSendBuffer() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].dynamicSendBufferUsing.fetch_add(1);
}

void IO_Metric::RecordDisconn() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].disconn.fetch_add(1);
}
}  // namespace sh::IO_Engine