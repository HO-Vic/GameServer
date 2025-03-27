#pragma once
#include "IO_Engine/IO_Core/IO_Core.h"
#include "IO_Engine/IO_Core/Acceptor/Acceptor.h"
#include "IO_Engine/IO_Core/Listener/Listener.h"
#include "../MsgDispatcher/MsgDispatcher.h"

namespace sh::IO_Engine {
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
class Server {
 public:
  Server(const uint8_t ioThreadNo);

  void Init();

  void Start();

 private:
  void AcceptHandle(SOCKET sock);

  void RecvHandle(sh::IO_Engine::ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition);

  static void OnLogin(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message);

  static void OnMatchReq(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message);

  static void OnCancelMatch(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message);

    static void OnStressDelay(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message);

 private:
  sh::IO_Engine::IO_Core m_ioCore;
  sh::IO_Engine::Listener m_listener;
  sh::IO_Engine::Acceptor m_acceptor;

  MsgDispatcher m_dispatcher;
};
}  // namespace DreamWorld
