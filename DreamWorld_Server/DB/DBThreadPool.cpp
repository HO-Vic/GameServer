#include "stdafx.h"
#include "DBThreadPool.h"
#include "../LogManager/LogManager.h"

using logLevel = spdlog::level::level_enum;
namespace DreamWorld {
void DBThreadPool::Init(const uint8_t threadNo) {
  WRITE_LOG(logLevel::info, "{}({}) > Init!", __FUNCTION__, __LINE__);
  m_threadPool.Init(threadNo);
}

}  // namespace DreamWorld
