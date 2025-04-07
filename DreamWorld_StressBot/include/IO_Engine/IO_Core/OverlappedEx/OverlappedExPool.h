#pragma once
#include <Utility/Pool/ObjectPool.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
class OverlappedExPool
    : public Utility::RawObjectPool<OverlappedEx>,
      public Utility::SingletonBase<OverlappedExPool> {
};
}  // namespace sh::IO_Engine
