#include <pch.h>
#include <Session/ISession.h>
#include <Session/SessionImpl.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

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

void ISession::DoSend(const void* data, const size_t len) {
  m_sessionImpl->DoSend(shared_from_this(), reinterpret_cast<const BYTE*>(data), len);
}

void ISession::StartRecv() {
  m_sessionImpl->StartRecv(shared_from_this());
}

void ISession::Execute(Utility::ThWorkerJob* thWorkerJob, const DWORD ioByte) {
  static constexpr bool DESIRE_DISCONNECT = true;
  bool CONNECTED = false;
  switch (thWorkerJob->GetType()) {
    case Utility::WORKER_TYPE::RECV: {
      m_sessionImpl->RecvComplete(thWorkerJob, ioByte);
    } break;
    case Utility::WORKER_TYPE::SEND: {
      m_sessionImpl->SendComplete(thWorkerJob, ioByte);
    } break;
    case Utility::WORKER_TYPE::DISCONN: {
      if (m_isDisconnnected.compare_exchange_strong(CONNECTED, DESIRE_DISCONNECT)) {  // 연결이 끊겼을 때, 여러 곳에서 Disconnect가 호출되더라도 오직 하나만 성공
        Disconnect();
      }
      // 어쨌든 오버랩 객체 회수 + 초기화 진행
      ThWorkerJobPool::GetInstance().Release(thWorkerJob);
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
