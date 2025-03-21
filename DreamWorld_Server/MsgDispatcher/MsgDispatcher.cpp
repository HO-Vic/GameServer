#include "stdafx.h"
#include "MsgDispatcher.h"
#include "../LogManager/LogManager.h"

namespace DreamWorld {
using logLevel = spdlog::level::level_enum;
bool MsgDispatcher::AddMsgHandler(const uint16_t msgId, MsgHandler handler) {
  if (m_dispatcher.contains(msgId)) {
    WRITE_LOG(logLevel::critical, "{}({}) > Duplicate MsgId!", __FUNCTION__, __LINE__);
    return false;
  }
  m_dispatcher.emplace(msgId, std::move(handler));
  WRITE_LOG(logLevel::debug, "{}({}) > Regist MsgId - Handler!", __FUNCTION__, __LINE__);
  return true;
}

bool MsgDispatcher::GetHandler(const uint16_t msgId, MsgHandler& handler) {
  if (!m_dispatcher.contains(msgId)) {
    WRITE_LOG(logLevel::warn, "{}({}) > Can not Find Msg Id!", __FUNCTION__, __LINE__);
    return false;
  }
  auto handlerIter = m_dispatcher.find(msgId);
  handler = handlerIter->second;
  return true;
}
}  // namespace DreamWorld