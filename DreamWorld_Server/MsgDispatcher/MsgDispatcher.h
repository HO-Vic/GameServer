#pragma once
#include <unordered_map>
#include <functional>
#include <Windows.h>
#include <memory>

namespace sh::IO_Engine {
class TCP_ISession;
using TCP_ISessionPtr = std::shared_ptr<TCP_ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
using MsgHandler = std::function<void(sh::IO_Engine::TCP_ISessionPtr, BYTE*)>;
class MsgDispatcher {
 public:
  bool AddMsgHandler(const uint8_t msgId, MsgHandler handler);

  bool GetHandler(const uint8_t msgId, MsgHandler& handler);

 private:
  std::unordered_map<uint8_t, MsgHandler> m_dispatcher;
};
}  // namespace DreamWorld
