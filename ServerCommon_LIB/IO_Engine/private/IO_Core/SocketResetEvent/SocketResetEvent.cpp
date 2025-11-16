#include "pch.h"
#include <IO_Core/SocketResetEvent/SocketResetEvent.h>
#include <cassert>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Core/Acceptor/AcceptEvent.h>

namespace sh::IO_Engine {
bool SocketResetEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
#ifdef _DEBUG
  assert(workerJob->GetType() == Utility::WORKER_TYPE::SOCKET_RECYCLE && "Worker type can own Only Socket Recycle");
#endif  // _DEBUG
  m_acceptEvent->InsertRecycleSocket(m_recycleSocket);
  ThWorkerJobPool::GetInstance().Release(workerJob);
  return true;
}
}  // namespace sh::IO_Engine