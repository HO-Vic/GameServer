#pragma once
#include <cstdint>
#include <string>
#include <winsock.h>
#include <Utility/SingletonBase/Singleton.h>
#include <IO_Engine/IO_Core/IO_Core.h>
#include <IO_Engine/IO_Core/Connector/AsyncConnector.h>
#include "../MsgDispatcher/MsgDispatcher.h"

namespace sh::IO_Engine {
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
}  // namespace sh::IO_Engine

namespace Stress {
constexpr sh::IO_Engine::ConnectorBase::MS TIMEOUT_MS = std::chrono::milliseconds(1000);
class NetworkModule final
    : public sh::Utility::SingletonBase<NetworkModule> {
 public:
  NetworkModule() {
  }

  ~NetworkModule() = default;

  void Init(const std::string& ipAddr, uint16_t port);

  void InitMsgDispatcher();

  void Start();

  void OnConnect(SOCKET sock);

  void OnConnectFail(int errorCode);

  void RecvHandle(sh::IO_Engine::ISessionPtr, size_t, BYTE*);

  // void Disconnect();
 private:
 private:
  sh::IO_Engine::IO_Core m_ioCore;
  sh::IO_Engine::AsyncConnector m_connector;
  MsgDispatcher m_msgDispatcher;
};
}  // namespace Stress
