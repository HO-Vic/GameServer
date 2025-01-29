#include <pch.h>
#include <Session/RecvContext/RecvContext.h>
#include <Session/RecvContext/TCP_RecvContextImpl.h>
#include <Session/RecvContext/UDP_RecvContextImpl.h>
#include <IO_Core/OverlappedEx/OverlappedExPool.h>

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

int32_t RecvContext::RecvComplete(OverlappedEx* overlappedEx, size_t ioSize) {
  auto errorNo = m_recvContextImpl->RecvComplete(overlappedEx, ioSize);
  if (WSA_IO_PENDING == errorNo) {
    errorNo = 0;
  }
  return errorNo;
}

int32_t RecvContext::StartRecv(OverlappedPtr& session) {
  auto overlappedEx = OverlappedExPool::GetInstance().GetObjectPtr(session, OVERLAPPED_EVENT_TYPE::RECV);
  auto errorNo = m_recvContextImpl->DoRecv(overlappedEx);
  if (WSA_IO_PENDING == errorNo) {
    errorNo = 0;
  }
  return errorNo;
}
}  // namespace sh::IO_Engine
