#pragma once
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <memory>
#include "../../CommonDefine.h"
/*
1. IOCP에 Listen 등록
2. Overlapped로 IOCP 워커에서 Accept 처리
*/

namespace sh::IO_Engine {
class AcceptEvent;
class Acceptor {
  union SocketAddress {  // 같은 메모리 레이 아웃 베이스인 형태
    sockaddr base;       // AF 2byte+ data 14byte
    sockaddr_in v4;      // AF 2byte + port 2byte + ~
    sockaddr_in6 v6;     // AF 2byte + port 2byte + ~
  };

 public:
  Acceptor() = default;

  Acceptor(AcceptCompleteHandler acceptCompleteHandle, const uint8_t acceptNo = 1);

  void Init(AcceptCompleteHandler acceptCompleteHandle, const uint8_t acceptNo = 1);

  // m_registToIocp: accept된 소켓에 대해서 바로 iocp에 등록할지에 대한
  int Start(HANDLE iocpHandle, const AddrConfig& acceptCfg, const SocketConfig& sockCfg, bool isNoDelay = true, bool m_registToIocp = true);

 private:
  SOCKET m_listenSocket = NULL;
  AcceptCompleteHandler m_acceptCompleteHandle = nullptr;
  std::vector<std::shared_ptr<AcceptEvent>> m_acceptEvents;
  uint8_t m_acceptNo = 1;
};
}  // namespace sh::IO_Engine
