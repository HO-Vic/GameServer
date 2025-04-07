#pragma once
#include <WinSock2.h>

#include "../../CommonDefine.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
class ConnectorBase {
 public:
  using clock = std::chrono::steady_clock;
  using Time = std::chrono::steady_clock::time_point;
  using MS = std::chrono::milliseconds;

 public:
  ConnectorBase(const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType);

  //
  virtual bool TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) = 0;

 protected:
  SOCKADDR_IN m_connectAddr;

  uint16_t m_inetType;
  int m_socketType;
  int m_protocolType;
};

class SyncConnector
    : public ConnectorBase {
 public:
  SyncConnector(const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType);

  virtual bool TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) override;
};

}  // namespace sh::IO_Engine
