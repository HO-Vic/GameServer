#include "pch.h"
#include "GlobalObjectPool.h"
#include <Utility/Job/Job.h>
#include <Utility/Timer/TimerJob.h>

void Stress::InitGlobalObjectPool() {
  Stress::GlobalObjectPool<sh::Utility::Job>::GetInstance().InitSize(1000);
  Stress::GlobalObjectPool<sh::Utility::TimerJob>::GetInstance().InitSize(1000);
}
