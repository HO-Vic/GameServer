#pragma once
#include <chrono>
#include <IO_Engine/IO_Core/Connector/Connector.h>
#include <IO_Engine/IO_Core/IO_Core.h>

namespace sh::IO_Engine {
class TCP_ISession;
using TCP_ISessionPtr = std::shared_ptr<TCP_ISession>;
}  // namespace sh::IO_Engine

namespace SimpleTCP {
using ms = std::chrono::milliseconds;
using sec = std::chrono::seconds;

class Session;

class NetworkAgent final {
 public:
  NetworkAgent(const uint8_t ioThreadNo, const std::string& ipAddr, const uint16_t portAddr);

  void Init();

  std::shared_ptr<Session> Connect(const ms timeOut);

  static void OnRecv(sh::IO_Engine::TCP_ISessionPtr session, uint64_t packetSize, BYTE* packetPtr);

 private:
  void OnConnSuccess(SOCKET sock);

  void OnConnFail(int errorCode);

 private:
  sh::IO_Engine::IO_Core m_core;
  sh::IO_Engine::SyncConnector m_connector;
  std::shared_ptr<Session> m_connSession;
  bool m_isConnSuccess = false;
};

}  // namespace SimpleTCP
