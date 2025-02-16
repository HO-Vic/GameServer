#include "pch.h"
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
void AcceptEvent::Start(OverlappedEx* overlappedEx) {
  ZeroMemory(&m_connInfo, sizeof(m_connInfo));
  m_clientSocket = WSASocket(m_inetType, m_socketType, m_protocolType, 0, 0, WSA_FLAG_OVERLAPPED);
  DWORD receiveByte = 0;
  bool isSuccess = AcceptEx(m_listenSocket, m_clientSocket, &m_connInfo, 0, sizeof(m_connInfo.localInfo), sizeof(m_connInfo.remoteInfo), &receiveByte, reinterpret_cast<LPOVERLAPPED>(overlappedEx));
}

void AcceptEvent::Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) {
  // Accept후처리
  auto connectedSocket = m_clientSocket;
  Start(overlappedEx);

  // m_clientSocket에 새 소켓 할당, AcceptEx() 호출 이후에 connectedSocket을 후처리 해야함
  m_acceptCompleteHandle(connectedSocket);
}
}  // namespace sh::IO_Engine
