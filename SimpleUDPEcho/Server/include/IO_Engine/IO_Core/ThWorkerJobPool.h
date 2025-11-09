#include "Utility/Pool/ObjectPool.h"
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThWorkerJob.h"

namespace sh::IO_Engine {
class ThWorkerJobPool
    : public Utility::SingletonBase<ThWorkerJobPool>,
      public Utility::RawObjectPool<sh::Utility::ThWorkerJob> {
 public:
  void Init(const uint32_t initSize) {
    InitSize(initSize);
  }
};
}  // namespace sh::IO_Engine
