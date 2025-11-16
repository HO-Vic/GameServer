#pragma once
#include <cstdint>
#include <Utility/Pool/ObjectPool.h>
#include <Utility/SingletonBase/Singleton.h>
#include "SocketResetEvent.h"

namespace sh::IO_Engine {
class SocketResetEventPool
    : public Utility::ObjectPool<SocketResetEvent>,
      public Utility::SingletonBase<SocketResetEventPool> {
 public:
  SocketResetEventPool() = default;

  void Init(uint32_t size, bool useMetric = false);
};
}  // namespace sh::IO_Engine
