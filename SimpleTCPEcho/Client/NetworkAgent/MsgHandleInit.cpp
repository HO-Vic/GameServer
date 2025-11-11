#include "pch.h"
#include "NetworkAgent.h"
#include "../Session/Session.h"
#include "../Packet/Packet.h"
#include "../LogMgr/LogManager.h"

namespace SimpleTCP {
void NetworkAgent::MsgInit() {
  using namespace std::placeholders;
  m_dispatcher.AddMsgHandle(PACKET_TYPE::SIMPLE_MSG, std::bind(NetworkAgent::OnSimpleMsg, _1, _2));

  m_dispatcher.AddMsgHandle(PACKET_TYPE::PARSED_MSG, std::bind(NetworkAgent::OnParsedMsg, _1, _2));
}
}  // namespace SimpleTCP