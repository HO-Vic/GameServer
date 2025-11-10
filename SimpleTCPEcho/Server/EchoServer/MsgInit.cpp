#include "PCH.h"
#include "EchoServer.h"
#include <functional>
#include "MsgDispatcher.h"
#include "Packet.h"

namespace SimpleTCP {
void Server::InitMsg() {
  using namespace std::placeholders;
  m_dispatcher.AddMsgHandle(PACKET_TYPE::SIMPLE_MSG, std::bind(&Server::OnSimpleMsg, _1, _2));
}
}  // namespace SimpleTCP