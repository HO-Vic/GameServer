#pragma once
#include <cstdint>
#include <string>
#include <winsock.h>
#include <chrono>
#include <Utility/SingletonBase/Singleton.h>
#include <IO_Engine/IO_Core/IO_Core.h>
#include <IO_Engine/IO_Core/Connector/AsyncConnector.h>
#include "../MsgDispatcher/MsgDispatcher.h"

#include <unordered_set>

namespace sh::IO_Engine {
class TCP_SessionBase;
using TCP_SessionBasePtr = std::shared_ptr<TCP_SessionBase>;
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

  void Init(const std::string& ipAddr, uint16_t port, const uint8_t ioThreadNo, const uint64_t sustainedDelayMs, const uint64_t burstDelayMs, const uint64_t sustainedWindowMs, const uint64_t adjustConnectDelayThreadshold);

  void InitMsgDispatcher();

  void Start();

  void OnConnect(SOCKET sock);

  void OnConnectFail(int errorCode);

  void RecvHandle(sh::IO_Engine::TCP_SessionBasePtr, size_t, BYTE*);

  // void Disconnect();
 private:
  void OnLoginSuccess(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

  static void OnIntoInGame(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

  static void OnGameState_Stage(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

  static void OnGameState_Boss(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

  static void OnGameEnd(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

  static void OnStressTestDelay(sh::IO_Engine::TCP_SessionBasePtr session, BYTE* packetHeader);

 private:
  sh::IO_Engine::IO_Core m_ioCore;
  sh::IO_Engine::AsyncConnector m_connector;
  MsgDispatcher m_msgDispatcher;

  MS m_connectDelayTick = MS(10);
  TIME::time_point m_lastTryConnTime;

  uint64_t m_sustainedDelayMs;
  uint64_t m_burstDelayMs;
  MS m_sustainedWindow{0};
  uint64_t m_adjustConnectDelayThreadshold;

 public:
  std::atomic_uint32_t g_connectUserCnt{};
  std::atomic_uint32_t g_ActiveUserCnt{};
  std::atomic_uint32_t g_maxConnectUserCnt{};

  std::atomic_uint64_t g_maxDelayTime{};  // ms
  std::atomic_uint64_t g_totalDelay{};    // ms
  std::atomic_uint64_t g_avgDelay{};      // ms
};
}  // namespace Stress
