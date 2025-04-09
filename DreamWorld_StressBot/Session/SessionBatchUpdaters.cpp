#include "pch.h"
#include "SessionBatchUpdaters.h"
#include <memory>
#include <Utility/Job/Job.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include "../GlobalObjectPool/GlobalObjectPool.h"
#include "SessionBatchUpdater.h"

namespace Stress {
void SessionBatchUpdaters::Init(const uint32_t threadCount, const uint8_t batchUpdaterCnt, const uint8_t timerThreadNo /*= 1*/) {
  m_threadPool.Init(threadCount);
  m_timer.Start(timerThreadNo);
  for (auto i = 0; i < batchUpdaterCnt; ++i) {
    m_batchUpdaters.push_back(std::make_shared<SessionBatchUpdater>());
    m_batchUpdaters.back()->ReserveNextUpdate();
  }
}

void SessionBatchUpdaters::InsertJobByIdx(const uint32_t idx, sh::Utility::Job::Caller&& caller) {
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(std::move(caller));
  m_batchUpdaters[idx % m_batchUpdaters.size()]->InsertJob(std::move(jobPtr));
}

std::shared_ptr<SessionBatchUpdater>& SessionBatchUpdaters::GetBatchUpdater(const uint32_t uniqueNo) {
  return m_batchUpdaters[uniqueNo % m_batchUpdaters.size()];
}

void SessionBatchUpdaters::InsertUpdaterTimerJob(const sh::Utility::WokerPtr& workerPtr) {
  auto workerJob = sh::IO_Engine::ThWorkerJobPool::GetInstance().GetObjectPtr(workerPtr, sh::Utility::WORKER_TYPE::WORK);
  PostQueuedCompletionStatus(m_threadPool.GetHandle(), 1, reinterpret_cast<ULONG_PTR>(workerPtr.get()), static_cast<LPOVERLAPPED>(workerJob));
}

}  // namespace Stress
