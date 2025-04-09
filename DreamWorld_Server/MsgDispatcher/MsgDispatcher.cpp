#include "stdafx.h"
#include "MsgDispatcher.h"
#include "../LogManager/LogManager.h"

namespace DreamWorld {
bool MsgDispatcher::AddMsgHandler(const uint8_t msgId, MsgHandler handler) {
  if (m_dispatcher.contains(msgId)) {
    WRITE_LOG(logLevel::critical, "{}({}) > Duplicate MsgId! [MsgId:{}]", __FUNCTION__, __LINE__, msgId);
    return false;
  }
  m_dispatcher.emplace(msgId, std::move(handler));
  WRITE_LOG(logLevel::debug, "{}({}) > Regist MsgId - Handler [MsgId:{}]!", __FUNCTION__, __LINE__, msgId);
  return true;
}

bool MsgDispatcher::GetHandler(const uint8_t msgId, MsgHandler& handler) {
  if (!m_dispatcher.contains(msgId)) {
    return false;
  }
  auto handlerIter = m_dispatcher.find(msgId);
  handler = handlerIter->second;
  return true;
}
}  // namespace DreamWorld