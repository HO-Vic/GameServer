#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include <pch.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <IO_Metric/IO_Metric.h>
#include <Session/TCP_ISession.h>
#include <Session/SendContext/TCP_SendContext.h>
#include <Session/RecvContext/TCP_RecvContext.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
TCP_ISession::TCP_ISession()
    : m_isDisconnnected(false), m_iocpHandle(NULL), m_sock(NULL) {
}
TCP_ISession::TCP_ISession(SOCKET sock, const IO_TYPE ioType, TCP_RecvHandler recvHandler, HANDLE iocpHandle)
    : m_sendContext(sock), m_recvContext(sock, std::move(recvHandler)), m_isDisconnnected(false), m_iocpHandle(iocpHandle), m_sock(sock) {
}

TCP_ISession::~TCP_ISession() {
  closesocket(m_sock);
}

void TCP_ISession::DefferedSet(SOCKET sock, TCP_RecvHandler recvHandler, HANDLE iocpHandle) {
  m_sendContext.DeferredSet(sock);
  m_recvContext.DefferedSet(sock, std::move(recvHandler));
  m_iocpHandle = iocpHandle;
  m_sock = sock;
}

void TCP_ISession::StartRecv() {
  auto thWorker = ThWorkerJobPool::GetInstance().GetObjectPtr(shared_from_this(), sh::Utility::WORKER_TYPE::RECV);
  auto ioError = m_recvContext.StartRecv(thWorker);  // 외부 정리
  if (0 != ioError) {
    RaiseIOError(thWorker);
  }
}

bool TCP_ISession::IsDisconnected() const {
  return m_isDisconnnected;
}

void TCP_ISession::RaiseIOError() {
  m_state.store(TCP_Session_STATE::DISCONNECT_STATE);
  PostQueuedCompletionStatus(m_iocpHandle, 1, 0, ThWorkerJobPool::GetInstance().GetObjectPtr(shared_from_this(), sh::Utility::WORKER_TYPE::DISCONN));
}

void TCP_ISession::RaiseIOError(sh::Utility::ThWorkerJob* thWorker) {
  m_state.store(TCP_Session_STATE::DISCONNECT_STATE);
  thWorker->SetType(sh::Utility::WORKER_TYPE::DISCONN);
  PostQueuedCompletionStatus(m_iocpHandle, 1, 0, thWorker);
}

void TCP_ISession::InternalSend(const BYTE* data, const uint32_t len) {
  if (m_state == TCP_Session_STATE::DISCONNECT_STATE) {
    return;
  }
  auto sendData = SendBufferAllocator::GetInstance().GetShared(len);
  sendData->Write(data, len);
  DoSend(std::move(sendData));
}

void TCP_ISession::Disconnect() {
}

void TCP_ISession::DoSend(std::shared_ptr<ISendBuffer>&& sendBuffer) {
  if (m_state == TCP_Session_STATE::DISCONNECT_STATE) {
    return;
  }
  auto ioError = m_sendContext.DoSend(shared_from_this(), std::move(sendBuffer));  // 실패시 Thworker는 내부 정리
  if (0 != ioError) {
    RaiseIOError();
  }
}

bool TCP_ISession::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  static constexpr bool DESIRE_DISCONNECT = true;
  if (m_state == TCP_Session_STATE::DISCONNECT_STATE && workerJob->GetType() != sh::Utility::WORKER_TYPE::DISCONN) {
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return true;
  }

  if (STATUS_SUCCESS != errorCode) {  // 0이 아니면 에러 발생
    m_state.store(TCP_Session_STATE::DISCONNECT_STATE);
    RaiseIOError(workerJob);
    return true;
  }

  bool expectedDisconn = false;
  bool returnVal = false;
  switch (workerJob->GetType()) {
    case Utility::WORKER_TYPE::RECV: {
      if (0 == ioByte) {
        RaiseIOError(workerJob);
        return returnVal;
      }
      auto ioError = m_recvContext.RecvComplete(workerJob, ioByte);  // thWorker외부 정리
      if (0 != ioError) {
        RaiseIOError(workerJob);
      }
      IO_Metric::GetInstance().RecordRecv(ioByte);
      returnVal = true;
    } break;
    case Utility::WORKER_TYPE::SEND: {
      auto ioError = m_sendContext.SendComplete(workerJob, ioByte);  // thWorker외부 정리
      if (0 != ioError) {
        RaiseIOError(workerJob);
      }
      IO_Metric::GetInstance().RecordSend(ioByte);
      returnVal = true;
    } break;
    case Utility::WORKER_TYPE::DISCONN: {
      if (m_isDisconnnected.compare_exchange_strong(expectedDisconn, DESIRE_DISCONNECT)) {  // 연결이 끊겼을 때, 여러 곳에서 Disconnect가 호출되더라도 오직 하나만 성공
        OnDisconnect();
        IO_Metric::GetInstance().RecordDisconn();
      }
      // 어쨌든 오버랩 객체 회수 + 초기화 진행
      ThWorkerJobPool::GetInstance().Release(workerJob);
      returnVal = true;
    } break;
    case Utility::WORKER_TYPE::FORCE_DISCONN: {
      RaiseIOError(workerJob);
      returnVal = true;
    } break;
    default: {
    } break;
  }
  return returnVal;
}
}  // namespace sh::IO_Engine
