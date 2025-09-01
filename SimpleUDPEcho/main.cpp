#include "pch.h"
#include "Server/UDP_Server.h"
#include "LogMgr/LogManager.h"
#include "Session/UDP_Session/UDP_Session.h"

int main() {
  START_LOGGER("UDP_Server", "Log/", "TestChat", spdlog::level::trace, "ConsoleFile");

  sh::UDP_Echo::UDP_Server Server{};
  if (!Server.Init(2)) {
    return -1;
  }
  Server.Start();
}