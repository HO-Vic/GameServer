#include "pch.h"
#include "MsgDispatcher.h"
#include "../LogMgr/LogManager.h"
#include <IO_Engine/Session/TCP_SessionBase.h>

namespace SimpleTCP {
void MsgDispatcher::AddMsgHandle(PACKET_TYPE type, MsgHandle msgHandle) {
  if (m_dispatcher.contains(type)) {
    WRITE_LOG(LogLevel::warn, "{}({}) > already Exist Handle, PacketType: {}", __FUNCTION__, __LINE__, static_cast<uint16_t>(type));
  }
  m_dispatcher.emplace(type, msgHandle);
  WRITE_LOG(LogLevel::info, "{}({}) > Regist MsgHandle, PacketType: {}", __FUNCTION__, __LINE__, static_cast<uint16_t>(type));
}

bool MsgDispatcher::GetHandle(PACKET_TYPE type, MsgHandle& msgHandle) {
  auto iter = m_dispatcher.find(type);
  if (iter == m_dispatcher.end()) {
    WRITE_LOG(LogLevel::warn, "{}({}) > Not Exist Handle, PacketType: {}", __FUNCTION__, __LINE__, static_cast<uint16_t>(type));
    return false;
  }
  msgHandle = iter->second;
  return true;
}
}  // namespace SimpleTCP