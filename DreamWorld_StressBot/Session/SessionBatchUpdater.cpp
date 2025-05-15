#include "pch.h"
#include "SessionBatchUpdater.h"
#include <type_traits>
#include <ioapiset.h>
#include <Windows.h>
#include <basetsd.h>
#include <cstdint>
#include <memory>
#include <Utility/Timer/TimerJob.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include <Utility/Thread/ThWorkerJob.h>
#include "SessionBatchUpdaters.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"
#include "../Session/Session.h"

namespace Stress {
void SessionBatchUpdater::InsertSession(SessionPtr session) {
  m_sessions.emplace(session->GetUniqueNo(), std::move(session));
}

void SessionBatchUpdater::DiscardSession(SessionPtr session) {
  m_sessions.erase(session->GetUniqueNo());
}

bool SessionBatchUpdater::Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
  IntenalUpdate();
  sh::IO_Engine::ThWorkerJobPool::GetInstance().Release(workerJob);
  ReserveNextUpdate();
  return true;
}

void SessionBatchUpdater::IntenalUpdate() {
  DoJobs();
  for (auto& [uniqueNo, session] : m_sessions) {
    session->Update();
  }
}

void SessionBatchUpdater::ReserveNextUpdate() {
  static constexpr sh::Utility::MS UPDATE_TICK = sh::Utility::MS(20);  // 초당 5번
  auto selfPtr = shared_from_this();
  SessionBatchUpdaters::GetInstance().GetTimer().InsertTimerEvent(std::move(
      Stress::GlobalObjectPool<sh::Utility::TimerJob>::GetInstance().MakeUnique(sh::Utility::_chrono_clock::now() + UPDATE_TICK, [selfPtr]() {
        SessionBatchUpdaters::GetInstance().InsertUpdaterTimerJob(selfPtr);
      })));
}
}  // namespace Stress
