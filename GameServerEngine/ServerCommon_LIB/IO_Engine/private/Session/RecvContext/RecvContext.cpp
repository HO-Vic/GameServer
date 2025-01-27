#include <pch.h>
#include <Session/RecvContext/RecvContext.h>
#include <Session/RecvContext/TCP_RecvContextImpl.h>
#include <Session/RecvContext/UDP_RecvContextImpl.h>

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

int32_t RecvContext::RecvCompletion(size_t ioSize) {
  return m_recvContextImpl->RecvCompletion(ioSize);
}

int32_t RecvContext::DoRecv() {
  return m_recvContextImpl->DoRecv();
}
}  // namespace sh::IO_Engine
