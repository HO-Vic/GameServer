#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Pool/ObjectPool.h"
#include "Utility/Job/Job.h"

namespace DreamWorld {
template <typename T>
class ObjectPool
    : public sh::Utility::ObjectPool<T>,
      public sh::Utility::SingletonBase<DreamWorld::ObjectPool<T>> {
};

void InitPool();
}  // namespace DreamWorld
