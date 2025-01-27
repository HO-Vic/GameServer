#pragma once
#include <pch.h>
#include <Utility/Pool/RawObjectPool.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
class OveralppedExPool
    : public Utility::RawObjectPool<OverlappedEx>,
      public Utility::SingletonBase<OveralppedExPool> {
 public:
  void Init(const uint32_t poolSize) {
    Utility::RawObjectPool<OverlappedEx>::Init(poolSize);
  }
};
}  // namespace sh::IO_Engine
