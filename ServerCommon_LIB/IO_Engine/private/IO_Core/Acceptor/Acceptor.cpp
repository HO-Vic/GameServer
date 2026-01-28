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

ErrorResult Acceptor::Start(HANDLE iocpHandle, const AddrConfig& acceptCfg, const SocketConfig& sockCfg, bool isNoDelay, bool registToIocp) {
  SocketAddress addrInfo{};
  addrInfo.base.sa_family = sockCfg.inetType;
  addrInfo.v4.sin_port = htons(acceptCfg.port);  // union이어서 v6랑 같은 메모리 위치

  uint64_t addrSize = sizeof(sockaddr_in);
  if (sockCfg.inetType == AF_INET) {
    auto result = inet_pton(sockCfg.inetType, acceptCfg.ip.c_str(), &addrInfo.v4.sin_addr);  // 성공하면 1을 반환
    if (result != 1) {
      return {ErrorType::WINSOCK_ERR, WSAGetLastError()};
    }
    addrSize = sizeof(sockaddr_in);
  } else if (sockCfg.inetType == AF_INET6) {
    auto result = inet_pton(sockCfg.inetType, acceptCfg.ip.c_str(), &addrInfo.v6.sin6_addr);  // 성공하면 1을 반환
    if (result != 1) {
      return {ErrorType::WINSOCK_ERR, WSAGetLastError()};
    }
    addrSize = sizeof(sockaddr_in6);
  }

  m_listenSocket = WSASocket(sockCfg.inetType, sockCfg.socketType, sockCfg.protocolType, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (m_listenSocket == INVALID_SOCKET) {
    return {ErrorType::WINSOCK_ERR, WSAGetLastError()};
  }

  // DisconnectEx 가져오기
  DWORD dwBytes = 0;
  GUID guid = WSAID_DISCONNECTEX;
  if (WSAIoctl(m_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
               &guid, sizeof(guid),
               &DisconnectEx, sizeof(DisconnectEx),
               &dwBytes, NULL, NULL) == SOCKET_ERROR) {
    closesocket(m_listenSocket);
    return {ErrorType::FailedDisconnectEx, WSAGetLastError()};
  }

  auto result = ::bind(m_listenSocket, &addrInfo.base, static_cast<int>(addrSize));
  if (result != 0) {  // 성공하면 0을 반환
    return {ErrorType::WINSOCK_ERR, WSAGetLastError()};
  }

  result = listen(m_listenSocket, SOMAXCONN);

  if (result != 0) {
    return {ErrorType::WINSOCK_ERR, WSAGetLastError()};
  }

  auto registResult = CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), iocpHandle, static_cast<ULONG_PTR>(m_listenSocket), 0);
  if (registResult == NULL) {
    return {ErrorType::WIN32_ERR, GetLastError()};
  }

  for (auto i = 0; i < m_acceptNo; ++i) {
    m_acceptEvents.push_back(std::make_shared<AcceptEvent>(m_listenSocket, iocpHandle, m_acceptCompleteHandle, sockCfg, isNoDelay, registToIocp));
    auto workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<Utility::IWorkerItem>(m_acceptEvents.back()), Utility::WORKER_TYPE::ACCEPT);
    m_acceptEvents.back()->Start(workerJob);
  }

  return {ErrorType::None, 0};
}
}  // namespace sh::IO_Engine
