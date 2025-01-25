#pragma once
#include <pch.h>
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include <Buffer/SendBuffer.h>
#include <stack>

namespace sh::IO_Engine {
class SendBufferPool : public Utility::ObjectPool<SendBuffer>, public Utility::SingletonBase<SendBufferPool> {
  constexpr static uint32_t INIT_SIZE = 100;

 public:
  SendBufferPool()
      : ObjectPool(INIT_SIZE) {
  }
};
}  // namespace sh::IO_Engine