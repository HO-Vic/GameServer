#include "pch.h"
#include <thread>
#include "LogManager/LogManager.h"
#include "DrawModule/DrawModule.h"
#include "NetworkModule/NetworkModule.h"
#include "GlobalObjectPool/GlobalObjectPool.h"
#include "Session/SessionBatchUpdaters.h"

int main() {
  START_LOGGER("StressTest", "DreamWorldBotLog/", "StressTest", static_cast<Stress::logLevel>(0), "ConsoleFile");
  Stress::InitGlobalObjectPool();
  Stress::SessionBatchUpdaters::GetInstance().Init(2, 4, 1);
  Stress::SessionBatchUpdaters::GetInstance().Start();
  std::thread thread([]() {
    auto& netModule = Stress::NetworkModule::GetInstance();
    netModule.Init("127.0.0.1", 9000);
    netModule.Start();
  });

  WinMain(0, 0, 0, 0);
  thread.join();
}
