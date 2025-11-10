#pragma once
#include <chrono>
#include <IO_Engine/IO_Core/Connector/Connector.h>
#include <IO_Engine/IO_Core/IO_Core.h>
#include "MsgDispatcher.h"

namespace SimpleTCP {
using ms = std::chrono::milliseconds;
using sec = std::chrono::seconds;

class Session;

class NetworkAgent final {
 public:
  NetworkAgent(const uint8_t ioThreadNo, const std::string& ipAddr, const uint16_t portAddr);

  void Init();

  void Start();

  void MsgInit();

  std::shared_ptr<Session> Connect(const ms timeOut);

  void OnRecv(SessionPtr session, uint64_t packetSize, BYTE* packetPtr);  // 얘 위치가 여기가 맞을까?

  static void OnSimpleMsg(SessionPtr sessionPtr, BYTE* packetHeader);

 private:
  void OnConnSuccess(SOCKET sock);

  void OnConnFail(int errorCode);

 private:
  sh::IO_Engine::IO_Core m_core;
  sh::IO_Engine::SyncConnector m_connector;
  std::shared_ptr<Session> m_connSession;
  MsgDispatcher m_dispatcher;
  bool m_isConnSuccess = false;
};

}  // namespace SimpleTCP
