#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include <WinSock2.h>
#include <ws2def.h>
#include <cstdint>
#include "../../CommonDefine.h"

namespace sh::IO_Engine {
class ThWorkerJob;
class AcceptEvent
    : public Utility::IWorkerItem {
 public:
  AcceptEvent(SOCKET listenSocket, AcceptCompleteHandler acceptHandle, uint16_t inetType = AF_INET, int socketType = SOCK_STREAM, int protocolType = IPPROTO_TCP)
      : m_listenSocket(listenSocket), m_clientSocket(NULL), m_acceptCompleteHandle(std::move(acceptHandle)), m_inetType(inetType), m_socketType(socketType), m_protocolType(protocolType) {
    ZeroMemory(&m_connInfo, sizeof(ConnectInfo));
  }

  void Start(Utility::ThWorkerJob* thWorkerJob);

  virtual bool Execute(Utility::ThWorkerJob* thWorkerJob, const DWORD ioByte, const uint64_t errorCode) override;

 private:
  SOCKET m_clientSocket;
  SOCKET m_listenSocket;
  ConnectInfo m_connInfo;
  AcceptCompleteHandler m_acceptCompleteHandle;
  int m_socketType;
  int m_protocolType;
  uint16_t m_inetType;
};
}  // namespace sh::IO_Engine
