#include <pch.h>
#include "EchoServer/EchoServer.h"
#include "ServerConfig/ServerConfig.h"
#include "LogManager/LogManager.h"

int main() {
  START_LOGGER("GameServer", "bin/Log/", "GameServer");

  sh::EchoServer::ServerConfig config;
  config.LoadXML("./config/ServerConfig.xml");

  sh::EchoServer::Server echoServer(config.GetIp(), config.GetPort(), config.GetThreadNo());

  echoServer.Init();

  echoServer.Start();

  return 0;
}
