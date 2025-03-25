#include "stdafx.h"
#include "DBThreadPool.h"
#include "../LogManager/LogManager.h"
#include <Utility/Thread/ThreadPool.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include "DBJobBase.h"

using logLevel = spdlog::level::level_enum;
namespace DreamWorld {
void DBThreadPool::InsertDBJob(std::shared_ptr<DBJobBase>&& jobPtr) {
  auto thWorkerjob = sh::IO_Engine::ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<sh::Utility::IWorkerItem>(jobPtr), sh::Utility::WORKER_TYPE::WORK);
  PostQueuedCompletionStatus(GetHandle(), 0, 0, thWorkerjob);
  // CreateTimerQueue - threadpoolleagacy?에 있는거인데, 함 확인해야할듯?
}

void DBThreadPool::Init(const uint8_t threadNo) {
  WRITE_LOG(logLevel::info, "{}({}) > Init!", __FUNCTION__, __LINE__);
  sh::Utility::ThreadPool::Init(threadNo);
}

}  // namespace DreamWorld
