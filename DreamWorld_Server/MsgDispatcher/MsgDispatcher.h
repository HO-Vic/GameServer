#pragma once
#include <unordered_map>

namespace sh::IO_Engine {
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
using MsgHandler = std::function<void(sh::IO_Engine::ISessionPtr, BYTE*)>;
class MsgDispatcher {
 public:
  bool AddMsgHandler(const uint16_t msgId, MsgHandler handler);

  bool GetHandler(const uint16_t msgId, MsgHandler& handler);

 private:
  std::unordered_map<uint16_t, MsgHandler> m_dispatcher;
};
}  // namespace DreamWorld
