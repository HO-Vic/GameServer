#include <pch.h>
#include <Session/Session.h>
#include <Session/SessionImpl.h>

namespace sh::IO_Engine {
Session::Session(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler)
    : m_sessionImpl(nullptr) {
  m_sessionImpl = new SessionImpl(sock, ioType, std::move(recvHandler));
}

Session::~Session() {
  delete m_sessionImpl;
}

void Session::DoSend(const BYTE* data, const size_t len) {
  m_sessionImpl->DoSend(shared_from_this(), data, len);
}

void Session::StartRecv() {
  m_sessionImpl->StartRecv(shared_from_this());
}
void Session::Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) {
  switch (type) {
    case RECV: {
      m_sessionImpl->RecvComplete(overlappedEx, ioByte);
    } break;
    case SEND: {
      m_sessionImpl->SendComplete(overlappedEx, ioByte);
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
