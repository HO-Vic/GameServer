#include <pch.h>
#include "EchoServer/EchoServer.h"
#include "ServerConfig/ServerConfig.h"
#include "LogManager/LogManager.h"
#include <Utility/BuildMsg/BuildMsg.h>

int main() {
  SimpleTCP::ServerConfig config;
  config.LoadXML("./config/ServerConfig.xml");

  START_LOGGER("GameServer", "Log/", "GameServer", static_cast<spdlog::level::level_enum>(config.GetLogLevel()), config.GetLogMode());

  SimpleTCP::Server echoServer(config.GetIp(), config.GetPort(), config.GetThreadNo());

  echoServer.Init();

  echoServer.Start();
  BUILD_MESSAGE(__FILE__, __LINE__, "재준이가 소고기 사주기로 했다😋🥩 (2025-08-30 05:34, 여름, 습합, 새벽)");
  BUILD_MESSAGE(__FILE__, __LINE__, "재준이가 '넥슨가면' 소고기 사주기로 했다😋🥩 (2025-08-30 05:39 수정)");
  return 0;
}
