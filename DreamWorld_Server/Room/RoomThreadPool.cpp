#include "stdafx.h"
#include "RoomThreadPool.h"
#include <memory>
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Pool/ObjectPool.h>
#include "../Room/RoomBase.h"

namespace DreamWorld {
void DreamWorld::RoomThreadPool::Init(const uint8_t threadNo) {
  sh::Utility::ThreadPool::Init(threadNo);
}
void RoomThreadPool::InsertRoomUpdateEvent(std::shared_ptr<RoomBase> roomPtr) {
  auto thWorkerJob = sh::IO_Engine::ThWorkerJobPool::GetInstance().GetObjectPtr(
      std::static_pointer_cast<sh::Utility::IWorkerItem>(roomPtr), sh::Utility::WORKER_TYPE::WORK);
  PostQueuedCompletionStatus(GetHandle(), 1, reinterpret_cast<ULONG_PTR>(roomPtr.get()), static_cast<LPOVERLAPPED>(thWorkerJob));
}
}  // namespace DreamWorld
