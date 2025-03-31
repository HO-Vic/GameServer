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
  uint16_t ioThreadNo;
  uint16_t roomThreadNo;
  uint16_t timerThreadNo;
  uint16_t DBThreadNo;

  std::string dbIp;
  std::string dbPort;
  std::string dbName;
  std::string dbId;
  std::string dbpw;

  std::unordered_set<uint32_t> targetRoomIds;  // Room Update Tick 체크를 위한
  DreamWorld::SEC logTickSec;

  std::string logMode;
  uint8_t logLevel;
};
}  // namespace DreamWorld
