#pragma once
#include <cstdint>
#include <string>

namespace sh::EchoServer {
class ServerConfig {
 public:
  ServerConfig();

  void LoadXML(const char* configFile);

  const uint16_t GetPort() const;

  const uint32_t GetIp();

  const uint16_t GetThreadNo() const;

  const uint8_t GetLogLevel() const;

  const std::string& GetLogMode() const;

 private:
  std::string ip;
  uint16_t port;
  uint16_t threadNo;
  uint32_t ipAddr;
  std::string logMode;
  uint8_t logLevel;
};
}  // namespace sh::EchoServer
