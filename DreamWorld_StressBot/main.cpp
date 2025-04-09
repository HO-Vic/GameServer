#include "pch.h"
#include <thread>
#include "LogManager/LogManager.h"
#include "DrawModule/DrawModule.h"
#include "NetworkModule/NetworkModule.h"

int main() {
  START_LOGGER("StressTest", "DreamWorldLog/", "StressTest", static_cast<Stress::logLevel>(0), "ConsoleFile");

  std::thread thread([]() {
    auto& netModule = Stress::NetworkModule::GetInstance();
    netModule.Init("127.0.0.1", 9000);
    netModule.Start();
  });

  WinMain(0, 0, 0, 0);
  thread.join();
}
