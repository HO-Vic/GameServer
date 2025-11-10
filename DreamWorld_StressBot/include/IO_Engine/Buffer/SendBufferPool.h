#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include "SendBuffer.h"
#include "../IO_Metric/IO_Metric.h"

namespace sh::IO_Engine {
class SendBufferPool
    : public Utility::ObjectPool<SendBuffer>,
      public Utility::SingletonBase<SendBufferPool> {
 public:
  SendBufferPool() {
  }

  void Init(uint32_t initSize);
};

class SendBufferAllocator
    : public Utility::SingletonBase<SendBufferAllocator> {
  static constexpr uint32_t BUFFER_SIZE[] = {128, 256, 512, 1024};

 public:
  void Init(uint32_t initSize);

  std::shared_ptr<ISendBuffer> GetShared(uint32_t len);

  void RecordMertric(IO_MetricSlot& metricSlot);

 private:
  Utility::ObjectPool<TCP_SendBuffer<BUFFER_SIZE[0]>> smallPool;
  Utility::ObjectPool<TCP_SendBuffer<BUFFER_SIZE[1]>> mediumPool;
  Utility::ObjectPool<TCP_SendBuffer<BUFFER_SIZE[2]>> largePool;
  Utility::ObjectPool<TCP_SendBuffer<BUFFER_SIZE[3]>> xLargePool;
};

class UDP_SingleSendBufferPool
    : public Utility::SingletonBase<UDP_SingleSendBufferPool>,
      public Utility::ObjectPool<UDP_SingleSendBuffer> {
};
}  // namespace sh::IO_Engine