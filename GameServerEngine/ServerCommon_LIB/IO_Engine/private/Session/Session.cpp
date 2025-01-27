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
  m_sessionImpl->DoSend(data, len);
}

void Session::SendComplete(const size_t ioByte) {
  m_sessionImpl->SendComplete(ioByte);
}

void Session::RecvComplete(const size_t ioByte) {
  m_sessionImpl->RecvCompletion(ioByte);
}

void Session::DoRecv() {
  m_sessionImpl->DoRecv();
}
void Session::Execute(const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) {
  switch (type) {
    case RECV: {
      RecvComplete(ioByte);
    } break;
    case SEND: {
      SendComplete(ioByte);
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
