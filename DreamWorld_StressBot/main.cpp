#include "pch.h"
#include <thread>
#include "LogManager/LogManager.h"
#include "DrawModule/DrawModule.h"
#include "NetworkModule/NetworkModule.h"
#include "GlobalObjectPool/GlobalObjectPool.h"
#include "Session/SessionBatchUpdaters.h"
#include "StressConfig/StressConfig.h"

int main() {
  auto& config = Stress::Config::GetInstance();
  config.LoadXML("../../config/StressConfig.xml");
  START_LOGGER("StressTest", "DreamWorldBotLog/", "StressTest", static_cast<Stress::logLevel>(config.logLevel), config.logMode);
  Stress::InitGlobalObjectPool();
  Stress::SessionBatchUpdaters::GetInstance().Init(config.batchUpdaterThreadNo, config.batchUpdaterCnt, config.timerThreadNo);
  Stress::SessionBatchUpdaters::GetInstance().Start();
  std::thread thread([&config]() {
    auto& netModule = Stress::NetworkModule::GetInstance();
    netModule.Init(config.ip, config.port, config.ioThreadNo, config.decUserMaxDelayMs, config.adjustConnDelayMs);
    netModule.Start();
  });

  WinMain(0, 0, 0, 0);
  thread.join();
}
