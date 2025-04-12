#include <pch.h>
#include <Session/RecvContext/RecvContext.h>
#include <Session/RecvContext/TCP_RecvContextImpl.h>
#include <Session/RecvContext/UDP_RecvContextImpl.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
RecvContext::RecvContext(const IO_TYPE ioType, SOCKET sock, RecvHandler recvHandler)
    : m_recvContextImpl(nullptr) {
  if (IO_TYPE::UDP == ioType) {
    m_recvContextImpl = new UDP_RecvContextImpl(sock, std::move(recvHandler));
    return;
  }
  m_recvContextImpl = new TCP_RecvContextImpl(sock, std::move(recvHandler));
}

RecvContext::~RecvContext() {
  delete m_recvContextImpl;
}

int32_t RecvContext::RecvComplete(Utility::ThWorkerJob* thWorkerJob, DWORD ioSize) {
  auto errorNo = m_recvContextImpl->RecvComplete(thWorkerJob, ioSize);
  if (0 != errorNo) {
    auto wsaErr = WSAGetLastError();
    if (WSA_IO_PENDING == wsaErr) {
      errorNo = 0;
    } else {
      errorNo = wsaErr;
    }
  }
  return errorNo;
}

int32_t RecvContext::StartRecv(Utility::WorkerPtr& session) {
  auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::RECV);
  return m_recvContextImpl->DoRecv(thWorkerJob);
  
}
}  // namespace sh::IO_Engine
