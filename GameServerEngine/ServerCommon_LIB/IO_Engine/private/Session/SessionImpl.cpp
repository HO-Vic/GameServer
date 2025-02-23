#include <pch.h>
#include <Session/SessionImpl.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <IO_Core/OverlappedEx/OverlappedExPool.h>

namespace sh::IO_Engine {
SessionImpl::SessionImpl(SOCKET sock, const IO_TYPE ioType, RecvHandler&& recvHandler, HANDLE iocpHandle)
    : m_sendContext(sock, ioType), m_recvContext(ioType, sock, recvHandler), m_state(SESSION_STATE::NON_ERR), m_iocpHandle(iocpHandle) {
}

void SessionImpl::SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) {
  if (SESSION_STATE::NON_ERR == m_state) {
    if (0 == m_sendContext.SendComplete(overlappedEx, ioByte)) {
      return;
    }
  }
  m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::SEND_ERR));

  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    overlappedEx->m_type = OVERLAPPED_EVENT_TYPE::DISCONNECT;
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
    return;
  }

  overlappedEx->m_overlappedEvent = nullptr;
  OverlappedExPool::GetInstance().Release(overlappedEx);
}

void SessionImpl::DoSend(OverlappedPtr session, const BYTE* data, const size_t len) {
  if (SESSION_STATE::NON_ERR == m_state) {
    if (0 == m_sendContext.DoSend(session, data, len)) {
      return;
    }
  }
  m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::SEND_ERR));
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    auto overlappedEx = OverlappedExPool::GetInstance().GetObjectPtr(session, OVERLAPPED_EVENT_TYPE::DISCONNECT);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
    return;
  }
}

void SessionImpl::RecvComplete(OverlappedEx* overlappedEx, size_t ioByte) {
  if (SESSION_STATE::NON_ERR != m_state) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
    // post
  }
  if (0 == ioByte) {  // Session 종료
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::DISCONNECT_STATE));
    overlappedEx->m_type = OVERLAPPED_EVENT_TYPE::DISCONNECT;
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
    return;
  }
  int32_t recvError = m_recvContext.RecvComplete(overlappedEx, ioByte);
  if (0 != recvError) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
  }
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    overlappedEx->m_type = OVERLAPPED_EVENT_TYPE::DISCONNECT;
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
  }
}

void SessionImpl::StartRecv(OverlappedPtr session) {
  int32_t recvError = m_recvContext.StartRecv(session);
  if (0 != recvError) {
    m_state.store(static_cast<SESSION_STATE>(m_state.load() | SESSION_STATE::RECV_ERR));
  }
  if (SESSION_STATE::DISCONNECT_STATE == m_state) {
    auto overlappedEx = OverlappedExPool::GetInstance().GetObjectPtr(session, OVERLAPPED_EVENT_TYPE::DISCONNECT);
    PostQueuedCompletionStatus(m_iocpHandle, 1, 0, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
  }
}
}  // namespace sh::IO_Engine