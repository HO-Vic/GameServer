#include <pch.h>
#include <Session/RecvContext/RecvContext.h>
#include <Session/RecvContext/TCP_RecvContextImpl.h>
#include <Session/RecvContext/UDP_RecvContextImpl.h>
#include <IO_Core/OverlappedEx/OveralppedExPool.h>

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
  return m_recvContextImpl->RecvComplete(overlappedEx, ioSize);
}

int32_t RecvContext::StartRecv() {
  auto overlappedEx = OveralppedExPool::GetInstance().GetObjectPtr();
  return m_recvContextImpl->DoRecv(overlappedEx);
}
}  // namespace sh::IO_Engine
