#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include <WinSock2.h>
#include <cstdint>
#include <atomic>
#include "../../CommonDefine.h"

namespace sh::IO_Engine {
class ThWorkerJob;
class AcceptEvent
    : public Utility::IWorkerItem {
 public:
  AcceptEvent(SOCKET listenSocket, HANDLE iocpHandle, AcceptCompleteHandler acceptHandle, uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP, bool isNoDelay = true, const bool registToIocp = true)
      : m_listenSocket(listenSocket), m_iocpHandle(iocpHandle), m_clientSocket(NULL), m_acceptCompleteHandle(std::move(acceptHandle)), m_inetType(inetType), m_socketType(socketType), m_protocolType(protocolType), m_isNoDelay(isNoDelay), m_registToIocp(registToIocp) {
    ZeroMemory(&m_connInfo, sizeof(ConnectInfo));
  }

  void Start(Utility::ThWorkerJob* workerJob);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

 private:
  HANDLE m_iocpHandle = NULL;
  SOCKET m_clientSocket = NULL;
  SOCKET m_listenSocket = NULL;
  ConnectInfo m_connInfo{};
  AcceptCompleteHandler m_acceptCompleteHandle = nullptr;
  int m_socketType = SOCK_STREAM;
  int m_protocolType = IPPROTO_TCP;
  uint16_t m_inetType = AF_INET;
  const bool m_isNoDelay = true;
  const bool m_registToIocp = true;
  /*
  원래는 class A {static int a;};는 클래스 선언만으로 메모리 할당 불가
  그래서 cpp에서 정의를 해줘야 했음. A::a = 0;
  c++17 이후 도입된 문법
  class A{inline static int a;};
  inline을 통해, 선언안에서 정의 초기화 가능
  */
  inline static std::atomic_uint64_t uniqueNo = 0;
};
}  // namespace sh::IO_Engine
