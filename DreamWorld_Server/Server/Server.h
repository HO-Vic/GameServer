#pragma once
#include "IO_Engine/IO_Core/IO_Core.h"
#include "IO_Engine/IO_Core/Acceptor/Acceptor.h"
#include "IO_Engine/IO_Core/Listener/Listener.h"
#include "../MsgDispatcher/MsgDispatcher.h"

namespace sh::IO_Engine {
class TCP_ISession;
using TCP_ISessionPtr = std::shared_ptr<TCP_ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
class Server {
 public:
  Server(const uint8_t ioThreadNo = 0, const bool useIoMetric = false, const bool useMetric = false);

  void Init(const uint8_t ioThreadNo = 0, const uint32_t thWorkerPoolSize = 1500, const uint32_t sendBufferPoolSize = 1500);

  void Start();

 private:
  void AcceptHandle(SOCKET sock);

  void RecvHandle(sh::IO_Engine::TCP_ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition);

  static void OnLogin(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);

  static void OnMatchReq(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);

  static void OnStressDelay(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);

 private:
  sh::IO_Engine::IO_Core m_ioCore;
  sh::IO_Engine::Listener m_listener;
  sh::IO_Engine::Acceptor m_acceptor;
  uint32_t m_acceptorCnt;

  MsgDispatcher m_dispatcher;
};
}  // namespace DreamWorld
