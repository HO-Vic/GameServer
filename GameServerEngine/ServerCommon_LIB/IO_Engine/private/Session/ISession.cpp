#include <pch.h>
#include <Session/ISession.h>
#include <Session/SessionImpl.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <IO_Core/OverlappedEx/OverlappedExPool.h>

namespace sh::IO_Engine {
ISession::ISession()
    : m_sessionImpl(nullptr), m_isDisconnnected(false) {
}
ISession::ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle)
    : m_sessionImpl(nullptr), m_isDisconnnected(false) {
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
  static constexpr bool DESIRE_DISCONNECT = true;
  static bool CONNECTED = false;
  switch (type) {
    case RECV: {
      m_sessionImpl->RecvComplete(overlappedEx, ioByte);
    } break;
    case SEND: {
      m_sessionImpl->SendComplete(overlappedEx, ioByte);
    } break;
    case DISCONNECT: {
      if (m_isDisconnnected.compare_exchange_strong(CONNECTED, DESIRE_DISCONNECT)) {  // 연결이 끊겼을 때, 여러 곳에서 Disconnect가 호출되더라도 오직 하나만 성공
        Disconnect();
      }
      // 어쨌든 오버랩 객체 회수 + 초기화 진행
      overlappedEx->m_overlappedEvent = nullptr;
      OverlappedExPool::GetInstance().Release(overlappedEx);
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
