#pragma once
#include "stdafx.h"
#include "ObjectPools.h"
#include <Utility/Job/Job.h>
#include "../Timer/TimerJob.h"
#include "../Server/ServerConfig.h"
#include "../LogManager/LogManager.h"
#include "../DB/DBPlayerLogin.h"

namespace DreamWorld {
void InitPool() {
  WRITE_LOG(logLevel::info, "{}({}) > DreamWorld Init Pool!", __FUNCTION__, __LINE__);
  ObjectPool<sh::Utility::Job>::GetInstance().InitSize(DreamWorld::ServerConfig::GetInstance().jobPoolSize);
  ObjectPool<TimerJob>::GetInstance().InitSize(DreamWorld::ServerConfig::GetInstance().timerJobPoolSize);
  ObjectPool<DBPlayerLogin>::GetInstance().InitSize(1500);
}
}  // namespace DreamWorld
