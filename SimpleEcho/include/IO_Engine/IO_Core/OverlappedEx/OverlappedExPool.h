#pragma once
#include <Utility/Pool/RawObjectPool.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
class OverlappedExPool
    : public Utility::RawObjectPool<OverlappedEx>,
      public Utility::SingletonBase<OverlappedExPool> {
 public:
  void Init(const uint32_t poolSize) {
    Utility::RawObjectPool<OverlappedEx>::Init(poolSize);
  }
};
}  // namespace sh::IO_Engine
