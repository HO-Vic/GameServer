#include <pch.h>
#include <Session/ISession.h>
#include <Session/SessionImpl.h>

namespace sh::IO_Engine {
ISession::ISession()
    : m_sessionImpl(nullptr) {
}
ISession::ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle)
    : m_sessionImpl(nullptr) {
  m_sessionImpl = new SessionImpl(sock, ioType, std::move(recvHandler), iocpHandle);
}

ISession::~ISession() {
  delete m_sessionImpl;
}

void ISession::DoSend(const BYTE* data, const size_t len) {
  m_sessionImpl->DoSend(shared_from_this(), data, len);
}

void ISession::StartRecv() {
  m_sessionImpl->StartRecv(shared_from_this());
}
void ISession::Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) {
  switch (type) {
    case RECV: {
      m_sessionImpl->RecvComplete(overlappedEx, ioByte);
    } break;
    case SEND: {
      m_sessionImpl->SendComplete(overlappedEx, ioByte);
    } break;
    case DISCONNECT: {
      Disconnect();
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
