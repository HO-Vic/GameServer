#include "pch.h"
#include "Session.h"
#include <type_traits>
#include <Utility/Job/Job.h>
#include "SessionBatchUpdater.h"
#include "SessionBatchUpdaters.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"

namespace Stress {
Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo)
    : ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo), sh::Utility::DoubleJobQ_MT("Stress User") {
}

Session ::~Session() {
}

void Session::OnDisconnect() {
  auto batchUpdater = SessionBatchUpdaters::GetInstance().GetBatchUpdater(GetUniqueNo());
  auto selfPtr = std::static_pointer_cast<Session>(shared_from_this());
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([batchUpdater, selfPtr]() {
    batchUpdater->DiscardSession(selfPtr);
  });
  batchUpdater->InsertJob(std::move(jobPtr));
}

void Session::Update() {
}
}  // namespace Stress