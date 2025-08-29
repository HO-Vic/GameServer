#include "pch.h"
#include "Server/UDP_Server.h"
#include "LogMgr/LogManager.h"

int main() {
  START_LOGGER("UDP_Server", "Log/", "TestChat", spdlog::level::debug, "ConsoleFile");

  sh::UDP_Echo::UDP_Server Server{};
  Server.Init(2);
  Server.Start();
}