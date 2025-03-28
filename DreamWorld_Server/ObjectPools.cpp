#pragma once
#include "stdafx.h"
#include "ObjectPools.h"
#include <Utility/Job/Job.h>
#include "../LogManager/LogManager.h"

namespace DreamWorld {
void InitPool() {
  WRITE_LOG(logLevel::info, "{}({}) > DreamWorld Init Pool!", __FUNCTION__, __LINE__);
  ObjectPool<sh::Utility::Job>::GetInstance().InitSize(200);
}
}  // namespace DreamWorld
