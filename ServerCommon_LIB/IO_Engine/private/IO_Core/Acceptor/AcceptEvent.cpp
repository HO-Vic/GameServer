#include "pch.h"
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Utility/Thread/ThWorkerJob.h>

namespace sh::IO_Engine {
void AcceptEvent::Start(Utility::ThWorkerJob* workerJob) {
  ZeroMemory(&m_connInfo, sizeof(m_connInfo));
  m_clientSocket = NULL;
  {  // 소켓 풀에 있는 소켓 가져오기
    std::lock_guard<std::mutex> lg{m_lock};
    if (!m_socketPool.empty()) {
      m_clientSocket = m_socketPool.front();
      m_socketPool.pop();
    }
  }

  if (NULL == m_clientSocket) {
    m_clientSocket = WSASocket(m_sockCfg.inetType, m_sockCfg.socketType, m_sockCfg.protocolType, 0, 0, WSA_FLAG_OVERLAPPED);
  }

  if (m_isNoDelay) {
    int flag = 1;
    setsockopt(m_clientSocket, m_sockCfg.protocolType, TCP_NODELAY, (char*)&flag, sizeof(flag));
  }
  DWORD receiveByte = 0;
  bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), &receiveByte, reinterpret_cast<LPOVERLAPPED>(workerJob));
}

bool AcceptEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  if (0 != errorCode) {  // 에러인 경우, 다시 accept
    Start(workerJob);
    return true;
  }
  // Accept후처리

  // accept된 소켓 복사
  auto connectedSocket = m_clientSocket;
  // 새 소켓 할당 + 비동기 accept
  Start(workerJob);
  // accept된 소켓 후처리
  if (m_registToIocp) {  // iocp에 바로 등록해야하는 경우
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(connectedSocket), m_iocpHandle, uniqueNo.fetch_add(1, std::memory_order::relaxed), 0);
  }
  // m_clientSocket에 새 소켓 할당, AcceptEx() 호출 이후에 connectedSocket을 후처리 해야함
  m_acceptCompleteHandle(connectedSocket, static_pointer_cast<AcceptEvent>(shared_from_this()));
  return true;
}
void AcceptEvent::InsertRecycleSocket(SOCKET sock) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_socketPool.push(sock);
}
}  // namespace sh::IO_Engine
