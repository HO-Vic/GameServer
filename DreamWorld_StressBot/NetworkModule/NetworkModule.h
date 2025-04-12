#pragma once
#include <cstdint>
#include <string>
#include <winsock.h>
#include <chrono>
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
  using MS = std::chrono::milliseconds;
  using TIME = std::chrono::steady_clock;

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
  void OnLoginSuccess(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

  static void OnIntoInGame(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

  static void OnGameState_Stage(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

  static void OnGameState_Boss(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

  static void OnGameEnd(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

  static void OnStressTestDelay(sh::IO_Engine::ISessionPtr session, BYTE* packetHeader);

 private:
  sh::IO_Engine::IO_Core m_ioCore;
  sh::IO_Engine::AsyncConnector m_connector;
  MsgDispatcher m_msgDispatcher;

  MS m_connectDelayTick = MS(10);
  TIME::time_point m_lastTryConnTime;

 public:
  std::atomic_uint32_t g_connectUserCnt{};
  std::atomic_uint32_t g_ActiveUserCnt{};
  std::atomic_uint64_t g_maxDelayTime{};  // ms
  std::atomic_uint64_t g_totalDelay{};    // ms
  std::atomic_uint64_t g_avgDelay{};      // ms
};
}  // namespace Stress
