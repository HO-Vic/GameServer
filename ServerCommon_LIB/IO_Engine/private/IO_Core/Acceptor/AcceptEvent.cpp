#include "pch.h"
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Utility/Thread/ThWorkerJob.h>

namespace sh::IO_Engine {
void AcceptEvent::Start(Utility::ThWorkerJob* workerJob) {
  ZeroMemory(&m_connInfo, sizeof(m_connInfo));
  m_clientSocket = WSASocket(m_inetType, m_socketType, m_protocolType, 0, 0, WSA_FLAG_OVERLAPPED);
  if (m_isNoDelay) {
    int flag = 1;
    setsockopt(m_clientSocket, m_protocolType, TCP_NODELAY, (char*)&flag, sizeof(flag));
  }

  DWORD receiveByte = 0;
  bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), &receiveByte, reinterpret_cast<LPOVERLAPPED>(workerJob));
}

bool AcceptEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  // Accept후처리
  auto connectedSocket = m_clientSocket;
  Start(workerJob);

  // m_clientSocket에 새 소켓 할당, AcceptEx() 호출 이후에 connectedSocket을 후처리 해야함
  m_acceptCompleteHandle(connectedSocket);
  return true;
}
}  // namespace sh::IO_Engine
