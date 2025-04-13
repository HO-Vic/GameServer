#include <pch.h>
#include <Session/ISession.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Session/SendContext/TCP_SendContext.h>
#include <Session/RecvContext/TCP_RecvContext.h>
#include <Session/SendContext/UDP_SendContext.h>
#include <Session/RecvContext/UDP_RecvContext.h>

namespace sh::IO_Engine {
ISession::ISession()
    : m_sendContext(nullptr), m_recvContext(nullptr), m_isDisconnnected(false), m_iocpHandle(NULL), m_sock(NULL) {
}
ISession::ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle)
    : m_sendContext(nullptr), m_recvContext(nullptr), m_isDisconnnected(false), m_iocpHandle(iocpHandle), m_sock(sock) {
  if (IO_TYPE::TCP == ioType) {
    m_sendContext = std::make_unique<TCP_SendContext>(sock);
    m_recvContext = std::make_unique<TCP_RecvContext>(sock, std::move(recvHandler));
  } else {
    //
  }
}

ISession::~ISession() {
  closesocket(m_sock);
}

void ISession::DoSend(const void* data, const size_t len) {
  auto ioError = m_sendContext->DoSend(shared_from_this(), reinterpret_cast<const BYTE*>(data), len);  // 실패시 Thworker는 내부 정리
  if (0 != ioError) {
    RaiseIOError();
  }
}

void ISession::StartRecv() {
  auto thWorker = ThWorkerJobPool::GetInstance().GetObjectPtr(shared_from_this(), sh::Utility::WORKER_TYPE::RECV);
  auto ioError = m_recvContext->DoRecv(thWorker);  // 외부 정리
  if (0 != ioError) {
    RaiseIOError(thWorker);
  }
}

bool ISession::IsDisconnected() const {
  return m_isDisconnnected;
}

void ISession::RaiseIOError() {
  m_state.store(SESSION_STATE::DISCONNECT_STATE);
  PostQueuedCompletionStatus(m_iocpHandle, 1, 0, ThWorkerJobPool::GetInstance().GetObjectPtr(shared_from_this(), sh::Utility::WORKER_TYPE::DISCONN));
}

void ISession::RaiseIOError(sh::Utility::ThWorkerJob* thWorker) {
  m_state.store(SESSION_STATE::DISCONNECT_STATE);
  thWorker->SetType(sh::Utility::WORKER_TYPE::DISCONN);
  PostQueuedCompletionStatus(m_iocpHandle, 1, 0, thWorker);
}

void ISession::Disconnect() {
}

void ISession::Execute(Utility::ThWorkerJob* thWorkerJob, const DWORD ioByte, const uint64_t errorCode) {
  static constexpr bool DESIRE_DISCONNECT = true;
  if (m_state == SESSION_STATE::DISCONNECT_STATE && thWorkerJob->GetType() != sh::Utility::WORKER_TYPE::DISCONN) {
    ThWorkerJobPool::GetInstance().Release(thWorkerJob);
    return;
  }
  if (0 != errorCode) {  // 0이 아니면 에러 발생
    m_state.store(SESSION_STATE::DISCONNECT_STATE);
    RaiseIOError(thWorkerJob);
    return;
  }

  bool expectedDisconn = false;
  switch (thWorkerJob->GetType()) {
    case Utility::WORKER_TYPE::RECV: {
      if (0 == ioByte) {
        RaiseIOError(thWorkerJob);
        return;
      }
      auto ioError = m_recvContext->RecvComplete(thWorkerJob, ioByte);  // thWorker외부 정리
      if (0 != ioError) {
        RaiseIOError(thWorkerJob);
      }
      IO_MetricSlot::GetInstance().RecordRecv(ioByte);
    } break;
    case Utility::WORKER_TYPE::SEND: {
      auto ioError = m_sendContext->SendComplete(thWorkerJob, ioByte);  // thWorker외부 정리
      if (0 != ioError) {
        RaiseIOError(thWorkerJob);
      }
      IO_MetricSlot::GetInstance().RecordSend(ioByte);
    } break;
    case Utility::WORKER_TYPE::DISCONN: {
      if (m_isDisconnnected.compare_exchange_strong(expectedDisconn, DESIRE_DISCONNECT)) {  // 연결이 끊겼을 때, 여러 곳에서 Disconnect가 호출되더라도 오직 하나만 성공
        OnDisconnect();
        IO_MetricSlot::GetInstance().RecordDisconn();
      }
      // 어쨌든 오버랩 객체 회수 + 초기화 진행
      ThWorkerJobPool::GetInstance().Release(thWorkerJob);
    } break;
    case Utility::WORKER_TYPE::FORCE_DISCONN: {
      RaiseIOError(thWorkerJob);
    } break;
    default: {
    } break;
  }
}
}  // namespace sh::IO_Engine
