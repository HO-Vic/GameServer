#include <pch.h>
#include "EchoServer/EchoServer.h"
#include "ServerConfig/ServerConfig.h"
#include "LogManager/LogManager.h"

int main() {
  sh::EchoServer::ServerConfig config;
  config.LoadXML("./config/ServerConfig.xml");

  START_LOGGER("GameServer", "Log/", "GameServer", static_cast<spdlog::level::level_enum>(config.GetLogLevel()), config.GetLogMode());

  sh::EchoServer::Server echoServer(config.GetIp(), config.GetPort(), config.GetThreadNo());

  echoServer.Init();

  echoServer.Start();

  return 0;
}
