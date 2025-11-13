#include "pch.h"
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
void SendBufferPool::Init(uint32_t initSize, const bool useMetric /*= false*/) {
  InitSize(initSize, useMetric);
}

void SendBufferAllocator::Init(uint32_t initSize, const bool useMetric /*= false*/) {
  m_useMetric = useMetric;
  smallPool.InitSize(initSize, useMetric);
  mediumPool.InitSize(initSize, useMetric);
  largePool.InitSize(initSize, useMetric);
  xLargePool.InitSize(initSize, useMetric);
}

std::shared_ptr<SendBufferBase> SendBufferAllocator::GetShared(uint32_t len) {
  if (len + 2 <= BUFFER_SIZE[0]) {
    return smallPool.MakeShared();
  }
  if (len + 2 <= BUFFER_SIZE[1]) {
    return mediumPool.MakeShared();
  }
  if (len + 2 <= BUFFER_SIZE[2]) {
    return largePool.MakeShared();
  }
  if (len + 2 <= BUFFER_SIZE[3]) {
    return xLargePool.MakeShared();
  }
  return std::make_shared<DynamicSendBuffer>(len + 2);
}

void SendBufferAllocator::RecordMertric(IO_MetricSlot& metricSlot) {
  if (!m_useMetric) {
    return;
  }
  metricSlot.smallSendBufferTotal.store(smallPool.GetTotalCnt());
  metricSlot.smallSendBufferUsing.store(smallPool.GetUsingCnt());
  metricSlot.smallSendBufferAdd.store(smallPool.GetAddedCnt());

  metricSlot.mediumSendBufferTotal.store(mediumPool.GetTotalCnt());
  metricSlot.mediumSendBufferUsing.store(mediumPool.GetUsingCnt());
  metricSlot.mediumSendBufferAdd.store(mediumPool.GetAddedCnt());

  metricSlot.largeSendBufferTotal.store(largePool.GetTotalCnt());
  metricSlot.largeSendBufferUsing.store(largePool.GetUsingCnt());
  metricSlot.largeSendBufferAdd.store(largePool.GetAddedCnt());

  metricSlot.xLargeSendBufferTotal.store(xLargePool.GetTotalCnt());
  metricSlot.xLargeSendBufferUsing.store(xLargePool.GetUsingCnt());
  metricSlot.xLargeSendBufferAdd.store(xLargePool.GetAddedCnt());
}
}  // namespace sh::IO_Engine
