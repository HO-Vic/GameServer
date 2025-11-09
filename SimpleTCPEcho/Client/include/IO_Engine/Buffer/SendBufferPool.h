#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBuffer.h>

namespace sh::IO_Engine {
class SendBufferPool
    : public Utility::ObjectPool<SendBuffer>,
      public Utility::SingletonBase<SendBufferPool> {
 public:
  SendBufferPool() {
  }
};

class UDP_SingleSendBufferPool
    : public Utility::SingletonBase<UDP_SingleSendBufferPool>,
      public Utility::ObjectPool<UDP_SingleSendBuffer> {
};
}  // namespace sh::IO_Engine