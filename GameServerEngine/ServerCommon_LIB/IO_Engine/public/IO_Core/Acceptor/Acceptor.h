#pragma once
#include <pch.h>

/*
1. IOCP에 Listen 등록
2. Overlapped로 IOCP 워커에서 Accept 처리
*/

namespace sh::IO_Engine {
class AcceptEvent;
class Acceptor {
 public:
  Acceptor(HANDLE iocpHandle, SOCKET listenSocket, AcceptCompleteHandler acceptHandleFunc, const uint8_t acceptNo = 1);

  void Start(uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP);

 private:
  HANDLE m_iocpHandle;
  SOCKET m_listenSocket;
  // std::vector<std::shared_ptr<AcceptEvent>> m_acceptEvents;
  AcceptCompleteHandler m_acceptCompleteHandle;
  uint8_t m_acceptNo;
};
}  // namespace sh::IO_Engine
