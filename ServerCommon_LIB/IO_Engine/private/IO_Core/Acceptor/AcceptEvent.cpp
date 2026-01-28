#include "pch.h"
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Utility/Thread/ThWorkerJob.h>

namespace sh::IO_Engine {
AcceptEvent::AcceptEvent(SOCKET listenSocket, HANDLE iocpHandle, AcceptCompleteHandler acceptHandle, const SocketConfig& sockCfg, bool isNoDelay, const bool registToIocp, uint32_t initSockSize)
    : m_listenSocket(listenSocket), m_iocpHandle(iocpHandle), m_clientSocket(NULL), m_acceptCompleteHandle(std::move(acceptHandle)), m_sockCfg(sockCfg), m_isNoDelay(isNoDelay), m_registToIocp(registToIocp) {
  ZeroMemory(&m_connInfo, sizeof(ConnectInfo));
  for (uint32_t i = 0; i < initSockSize; ++i) {
    auto sock = WSASocket(m_sockCfg.inetType, m_sockCfg.socketType, m_sockCfg.protocolType, 0, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
      continue;
    }
    m_socketPool.push(sock);
  }
}

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
  if (INVALID_SOCKET == m_clientSocket) {
    // 즉시 재시도하면 busy-loop가 될 수 있으므로 약간의 지연 후 재시도
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    workerJob->SetType(Utility::WORKER_TYPE::RETRY_ACCEPT);  // 재시도 하는 type로 변경하여 수행
    PostQueuedCompletionStatus(m_iocpHandle, 0, 0, reinterpret_cast<LPOVERLAPPED>(workerJob));
    return;
  }
  if (m_isNoDelay) {
    int flag = 1;
    setsockopt(m_clientSocket, m_sockCfg.protocolType, TCP_NODELAY, (char*)&flag, sizeof(flag));
  }
  DWORD receiveByte = 0;
  workerJob->SetType(Utility::WORKER_TYPE::ACCEPT);
  bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), &receiveByte, reinterpret_cast<LPOVERLAPPED>(workerJob));
}

bool AcceptEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  if (workerJob->GetType() == Utility::WORKER_TYPE::ACCEPT) {
    // Accept후처리
    if (0 == errorCode) {  // 에러인 경우, 다시 accept
      // 새 소켓 할당 + 비동기 accept
      // accept된 소켓 후처리
      if (m_registToIocp) {  // iocp에 바로 등록해야하는 경우
        CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_iocpHandle, uniqueNo.fetch_add(1, std::memory_order::relaxed), 0);
      }
      // m_clientSocket에 새 소켓 할당, AcceptEx() 호출 이후에 m_clientSocket을 후처리 해야함
      m_acceptCompleteHandle(m_clientSocket, static_pointer_cast<AcceptEvent>(shared_from_this()));
    }
  }
  Start(workerJob);  // Retry인 경우, 똑같이 Start 타야 함
  return true;
}
void AcceptEvent::InsertRecycleSocket(SOCKET sock) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_socketPool.push(sock);
}
}  // namespace sh::IO_Engine
