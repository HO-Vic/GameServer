#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "../Packet/Packet.h"

namespace sh::IO_Engine {
class TCP_SessionBase;
}  // namespace sh::IO_Engine

namespace SimpleTCP {
using SessionPtr = std::shared_ptr<sh::IO_Engine::TCP_SessionBase>;
using MsgHandle = std::function<void(SessionPtr, BYTE*)>;

class MsgDispatcher {
 public:
  MsgDispatcher() = default;

  void AddMsgHandle(PACKET_TYPE type, MsgHandle msgHandle);

  bool GetHandle(PACKET_TYPE type, MsgHandle& msgHandle);

 private:
  std::unordered_map<PACKET_TYPE, MsgHandle> m_dispatcher;
};

}  // namespace SimpleTCP
