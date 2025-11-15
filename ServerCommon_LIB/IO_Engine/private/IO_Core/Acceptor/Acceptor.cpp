#include "pch.h"
#include <IO_Core/Acceptor/Acceptor.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <memory>
#include <functional>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <CommonDefine.h>

namespace sh::IO_Engine {
Acceptor::Acceptor(AcceptCompleteHandler acceptCompleteHandle, const uint8_t acceptNo)
    : m_acceptCompleteHandle(std::move(acceptCompleteHandle)), m_acceptNo(acceptNo) {
}

void Acceptor::Init(AcceptCompleteHandler acceptHandleFunc, const uint8_t acceptNo) {
  m_acceptCompleteHandle = std::move(acceptHandleFunc);
  m_acceptNo = acceptNo;
}

int Acceptor::Start(HANDLE iocpHandle, const AddrConfig& acceptCfg, const SocketConfig& sockCfg, bool isNoDelay, bool m_registToIocp) {
  SocketAddress addrInfo{};
  addrInfo.base.sa_family = sockCfg.inetType;
  addrInfo.v4.sin_port = htons(acceptCfg.port);  // union이어서 v6랑 같은 메모리 위치

  uint64_t addrSize = sizeof(sockaddr_in);
  if (sockCfg.inetType == AF_INET) {
    auto result = inet_pton(sockCfg.inetType, acceptCfg.ip.c_str(), &addrInfo.v4.sin_addr);  // 성공하면 1을 반환
    if (result != 1) {
      return WSAGetLastError();
    }
    addrSize = sizeof(sockaddr_in);
  } else if (sockCfg.inetType == AF_INET6) {
    auto result = inet_pton(sockCfg.inetType, acceptCfg.ip.c_str(), &addrInfo.v6.sin6_addr);  // 성공하면 1을 반환
    if (result != 1) {
      return WSAGetLastError();
    }
    addrSize = sizeof(sockaddr_in6);
  }

  m_listenSocket = WSASocket(sockCfg.inetType, sockCfg.socketType, sockCfg.protocolType, NULL, 0, WSA_FLAG_OVERLAPPED);
#ifdef _DEBUG
  assert(m_listenSocket != INVALID_SOCKET);
#endif  // _DEBUG

  auto result = ::bind(m_listenSocket, &addrInfo.base, static_cast<int>(addrSize));
  if (result != 0) {  // 성공하면 1을 반환
    return WSAGetLastError();
  }

  result = listen(m_listenSocket, SOMAXCONN);

  if (result != 0) {
    return WSAGetLastError();
  }

  auto registResult = CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), iocpHandle, static_cast<ULONG_PTR>(m_listenSocket), 0);
  if (registResult == NULL) {
    return INT32_MAX;
  }

  for (auto i = 0; i < m_acceptNo; ++i) {
    auto acceptEvent = std::make_shared<AcceptEvent>(m_listenSocket, iocpHandle, m_acceptCompleteHandle, sockCfg, isNoDelay, m_registToIocp);
    auto workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<Utility::IWorkerItem>(acceptEvent), Utility::WORKER_TYPE::ACCEPT);
    acceptEvent->Start(workerJob);
  }

  return 0;
}
}  // namespace sh::IO_Engine
