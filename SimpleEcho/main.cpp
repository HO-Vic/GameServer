#include <pch.h>
#include "EchoServer/EchoServer.h"
#include "ServerConfig/ServerConfig.h"

int main() {
  sh::EchoServer::ServerConfig config;
  config.LoadXML("./config/ServerConfig.xml");

  sh::EchoServer::EchoServer echoServer(config.GetIp(), config.GetPort(), config.GetThreadNo());
  echoServer.Start();

  return 0;
}
