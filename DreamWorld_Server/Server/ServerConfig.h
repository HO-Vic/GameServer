#pragma once
#include <cstdint>
#include <string>
#include <Utility/SingletonBase/Singleton.h>

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

  std::string logMode;
  uint8_t logLevel;
};
}  // namespace DreamWorld
