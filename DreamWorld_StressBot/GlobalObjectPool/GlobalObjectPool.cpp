#include "pch.h"
#include "GlobalObjectPool.h"
#include <Utility/Job/Job.h>
#include "../Timer/TimerJob.h"

namespace Stress {
void InitGlobalObjectPool() {
  GlobalObjectPool<sh::Utility::Job>::GetInstance().InitSize(1000);
  GlobalObjectPool<TimerJob>::GetInstance().InitSize(1000);
}
}  // namespace Stress
