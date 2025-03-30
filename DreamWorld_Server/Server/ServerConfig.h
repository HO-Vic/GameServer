#pragma once
#include <cstdint>
#include <string>

namespace DreamWorld {
class ServerConfig {
 public:
  ServerConfig();

  void LoadXML(const char* configFile);

 private:
  uint32_t ipAddr;
  std::string ip;
  uint16_t port;
  uint16_t ioThreadNo;
  uint16_t roomThreadNo;
  uint16_t DBThreadNo;


  std::string logMode;
  uint8_t logLevel;
};
}  // namespace DreamWorld
