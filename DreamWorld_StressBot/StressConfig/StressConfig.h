#pragma once
#include <cstdint>
#include <string>
#include <unordered_set>
#include <Utility/SingletonBase/Singleton.h>

namespace Stress {
class Config
    : public sh::Utility::SingletonBase<Config> {
 public:
  Config();

  void LoadXML(const char* configFile);

  const uint32_t GetServerIp();

  uint32_t ipAddr;
  std::string ip;
  uint16_t port;
  uint8_t ioThreadNo;
  uint8_t batchUpdaterThreadNo;
  uint8_t timerThreadNo;
  uint8_t batchUpdaterCnt;

  uint64_t decUserMaxDelayMs;
  uint64_t adjustConnDelayMs;

  std::string logMode;
  uint8_t logLevel;
};
}  // namespace Stress
