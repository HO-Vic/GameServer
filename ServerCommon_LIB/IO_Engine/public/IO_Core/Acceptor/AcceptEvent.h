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
  AcceptEvent(SOCKET listenSocket, HANDLE iocpHandle, AcceptCompleteHandler acceptHandle, const SocketConfig& sockCfg, bool isNoDelay = true, const bool registToIocp = true, uint32_t initSockSize = 1000);

  void Start(Utility::ThWorkerJob* workerJob);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

  void InsertRecycleSocket(SOCKET sock);

 private:
  HANDLE m_iocpHandle = NULL;  // accept된 소켓에 대한 등록
  SOCKET m_clientSocket = NULL;
  SOCKET m_listenSocket = NULL;  // 다시 acceptEx호출을 위한
  ConnectInfo m_connInfo{};
  AcceptCompleteHandler m_acceptCompleteHandle = nullptr;
  std::mutex m_lock;
  std::queue<SOCKET> m_socketPool;
  SocketConfig m_sockCfg{};
  const bool m_isNoDelay = true;
  const bool m_registToIocp = true;
  /*
  원래는 class A {static int a;};는 클래스 선언만으로 메모리 할당 불가
  그래서 cpp에서 정의를 해줘야 했음. A::a = 0;
  c++17 이후 도입된 문법
  class A{inline static int a;};
  inline을 통해, 선언안에서 정의 초기화 가능

  ##
  만약에 다른 acceptor에대한 AcceptEvent도 이 uniqueNo를 공유해도 문제 없을듯?
  */
  inline static std::atomic_uint64_t uniqueNo = 0;
};
}  // namespace sh::IO_Engine
