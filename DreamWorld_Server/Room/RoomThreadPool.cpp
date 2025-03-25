#include "stdafx.h"
#include "RoomThreadPool.h"

namespace DreamWorld {
void DreamWorld::RoomThreadPool::Init(const uint8_t threadNo) {
  sh::Utility::ThreadPool::Init(threadNo);
}
}  // namespace DreamWorld
