#pragma once
#include <Utility/Pool/ObjectPool.h>
#include <Utility/SingletonBase/Singleton.h>

namespace Stress {
template <typename T>
class GlobalObjectPool
    : public sh::Utility::ObjectPool<T>,
      public sh::Utility::SingletonBase<GlobalObjectPool<T>> {
};

void InitGlobalObjectPool();

}  // namespace Stress
