#pragma once
#include <cstdint>
#include <string>
#include <unordered_set>
#include <Utility/SingletonBase/Singleton.h>
#include "../DreamWorldDefine.h"

namespace DreamWorld {
class ServerConfig
    : public sh::Utility::SingletonBase<ServerConfig> {
 public:
  ServerConfig();

  void LoadXML(const char* configFile);

  const uint32_t GetServerIp();

  uint32_t ipAddr;
  std::string ip;
  uint16_t port;
  uint8_t ioThreadNo;
  uint8_t roomThreadNo;
  uint8_t timerThreadNo;
  uint8_t DBThreadNo;

  std::string dbIp;
  std::string dbPort;
  std::string dbName;
  std::string dbId;
  std::string dbpw;

#pragma region Metric
  std::unordered_set<uint32_t> targetRoomIds;  // Room Update Tick 체크를 위한
  DreamWorld::SEC targetRoomLogTickSec;

  DreamWorld::SEC avgRoomLogTickSec;

  uint32_t maxRoomTickMs;
  uint32_t reactiveTickMs;

  bool useIOMetric;

  bool useGameMetric;

  DreamWorld::SEC meticLoggingTickSec;
#pragma endregion

#pragma region PoolSize
  uint32_t thWorkerPoolSize = 5000;
  uint32_t sendBufferPoolSize = 2000;
  uint32_t jobPoolSize = 700;
  uint32_t timerJobPoolSize = 700;
#pragma endregion

  std::string logMode;
  uint8_t logLevel;

  bool isLocal = false;
  uint8_t testPeople = 0;  // 0이면 정상 작동
};
}  // namespace DreamWorld
