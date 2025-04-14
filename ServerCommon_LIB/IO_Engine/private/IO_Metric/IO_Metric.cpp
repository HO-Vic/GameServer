#include "pch.h"
#include <IO_Metric/IO_Metric.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
void IO_MetricSlot::Init(bool isUse) {
  m_isUse = isUse;
}

IO_Metric& IO_MetricSlot::SwapAndLoad() {
  // Swap하기전에 오브젝트 풀 사용량 기록을 먼저
  auto taskIndex = m_index.load();
  {
    m_metics[taskIndex].thWorkerJobTotal.store(ThWorkerJobPool::GetInstance().GetTotalCnt());
    m_metics[taskIndex].thWorkerJobUsing.store(ThWorkerJobPool::GetInstance().GetUsingCnt());
    m_metics[taskIndex].thWorkerJobAdd.store(ThWorkerJobPool::GetInstance().GetAddedCnt());

    m_metics[taskIndex].sendBufferTotal.store(SendBufferPool::GetInstance().GetTotalCnt());
    m_metics[taskIndex].sendBufferUsing.store(SendBufferPool::GetInstance().GetUsingCnt());
    m_metics[taskIndex].sendBufferAdd.store(SendBufferPool::GetInstance().GetAddedCnt());
  }
  auto otherIndex = (taskIndex + 1) % 2;
  m_metics[otherIndex].Reset();
  m_index.store(otherIndex);

  return m_metics[taskIndex];
}

void IO_MetricSlot::RecordSend(const DWORD ioByte) {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].sendCompletion.fetch_add(1);
  m_metics[taskIndex].sendByte.fetch_add(ioByte);
}

void IO_MetricSlot::RecordRecv(const DWORD ioByte) {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].recvCompletion.fetch_add(1);
  m_metics[taskIndex].recvByte.fetch_add(ioByte);
}

void IO_MetricSlot::RecordDisconn() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].disconn.fetch_add(1);
}
}  // namespace sh::IO_Engine