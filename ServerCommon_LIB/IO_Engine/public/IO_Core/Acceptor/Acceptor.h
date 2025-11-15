#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <cstdint>
#include "./AcceptEvent.h"
#include "../../CommonDefine.h"
/*
1. IOCP에 Listen 등록
2. Overlapped로 IOCP 워커에서 Accept 처리
*/

namespace sh::IO_Engine {
class AcceptEvent;
class Acceptor {
 public:
  Acceptor();

  Acceptor(AcceptCompleteHandler acceptHandleFunc, const bool registToIocp = true, const uint8_t acceptNo = 1);

  void Init(AcceptCompleteHandler acceptHandleFunc, const bool registToIocp = true, const uint8_t acceptNo = 1);

  void Start(SOCKET listenSocket, HANDLE iocpHandle, uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP, bool isNoDelay = true);

 private:
  // std::vector<std::shared_ptr<AcceptEvent>> m_acceptEvents;
  AcceptCompleteHandler m_acceptCompleteHandle;
  uint8_t m_acceptNo;
  bool m_registToIocp = true;  // accept된 소켓에 대해서 바로 iocp에 등록할지에 대한
};
}  // namespace sh::IO_Engine
