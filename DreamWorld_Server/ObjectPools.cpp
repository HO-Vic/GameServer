#pragma once
#include "stdafx.h"
#include "ObjectPools.h"
#include <Utility/Job/Job.h>

namespace DreamWorld {
void InitPool() {
  ObjectPool<sh::Utility::Job>::GetInstance().InitSize(200);
}
}  // namespace DreamWorld
