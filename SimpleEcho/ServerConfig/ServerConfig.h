#pragma once
#include <cstdint>
#include <string>

namespace sh::EchoServer {
class ServerConfig {
 public:
  ServerConfig();

  void LoadXML(const char* configFile);

  const uint16_t GetPort() const;

  const uint16_t GetIp();

  const uint16_t GetThreadNo() const;

 private:
  std::string ip;
  uint16_t port;
  uint16_t threadNo;
  uint32_t ipAddr;
};
}  // namespace sh::EchoServer
