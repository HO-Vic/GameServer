#include <pch.h>
#include <Session/SessionImpl.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
SessionImpl::SessionImpl(SOCKET sock, const IO_TYPE ioType, RecvHandler&& recvHandler, HANDLE iocpHandle)
    : m_sendContext(sock, ioType), m_recvContext(ioType, sock, recvHandler), m_state(SESSION_STATE::NON_ERR), m_iocpHandle(iocpHandle) {
}

void SessionImpl::SendComplete(Utility::ThWorkerJob* thWorkerJob, const DWORD ioByte) {
  if (SESSION_STATE::NON_ERR == m_state) {
    if (0 == m_sendContext.SendComplete(thWorkerJob, ioByte)) {
      return;
    }
  }
  m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::SEND_ERR));

  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    thWorkerJob->SetType(Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }
  ThWorkerJobPool::GetInstance().Release(thWorkerJob);
}

void SessionImpl::DoSend(Utility::WokerPtr session, const BYTE* data, const size_t len) {
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {  // 이미 바뀐건, 누군가 Disconnect 요청한거
    auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }

  auto sendErr = m_sendContext.DoSend(session, data, len);
  if (0 == sendErr) {
    return;
  }

  m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::SEND_ERR));
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }
}

void SessionImpl::RecvComplete(Utility::ThWorkerJob* thWorkerJob, DWORD ioByte) {
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {  // 이미 바뀐건, 누군가 Disconnect 요청한거
    thWorkerJob->SetType(Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }

  if (SESSION_STATE::NON_ERR != m_state) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
    thWorkerJob->SetType(Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }

  if (0 == ioByte) {  // Session 종료
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::DISCONNECT_STATE));
    thWorkerJob->SetType(Utility::WORKER_TYPE::DISCONN);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    return;
  }

  int32_t recvError = m_recvContext.RecvComplete(thWorkerJob, ioByte);
  if (0 != recvError) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
    if (SESSION_STATE::DISCONNECT_STATE == m_state) {  // 만약에 recv에러 없었지만, send에서 상태 바꿨다면 곤란한 상황이 생길 수 있음
      thWorkerJob->SetType(Utility::WORKER_TYPE::DISCONN);
      PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    }
  }
}

void SessionImpl::StartRecv(Utility::WokerPtr session) {
  int32_t recvError = m_recvContext.StartRecv(session);
  if (0 != recvError) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
    if (SESSION_STATE::DISCONNECT_STATE == m_state) {
      auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(session, Utility::WORKER_TYPE::DISCONN);
      PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(thWorkerJob));
    }
  }
}
}  // namespace sh::IO_Engine