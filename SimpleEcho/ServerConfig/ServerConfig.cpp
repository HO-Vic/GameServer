#include "pch.h"
#include "ServerConfig.h"
#include <rapidxml/rapidxml.hpp>
#include <inaddr.h>

namespace sh::EchoServer {
ServerConfig::ServerConfig()
    : ip("0.0.0.0"), port(9000), threadNo(4), ipAddr(0), logLevel(1) {
}

void ServerConfig::LoadXML(const char* configFile) {
  std::ifstream xmlFile(configFile);
  if (!xmlFile) {
    assert("invalid xml file");
  }

  std::vector<char> xmlDatas;
  std::copy(std::istreambuf_iterator<char>(xmlFile), std::istreambuf_iterator<char>(), std::back_inserter(xmlDatas));
  xmlDatas.push_back(static_cast<char>(0));
  std::unique_ptr<rapidxml::xml_document<char>> xmlDoc = std::make_unique<rapidxml::xml_document<char>>();
  xmlDoc->parse<0>(&xmlDatas.front());

  if (xmlDoc->first_node() != nullptr) {
    for (auto root = xmlDoc->first_node(); root != nullptr; root = root->next_sibling()) {
      if (strcmp(root->name(), "Properties") == 0) {
        if (root->first_node() != nullptr) {
          for (auto node = root->first_node(); node != nullptr; node = node->next_sibling()) {
            if (strcmp(node->name(), "property") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "LogLevel") == 0) {
                  logLevel = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "LogMode") == 0) {
                  logMode = attr->value();
                }
              }
            }
          }
        }
      } else if (strcmp(root->name(), "EchoServer") == 0) {
        if (root->first_attribute() != nullptr) {
          for (auto attr = root->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
            if (strcmp(attr->name(), "ip") == 0) {
              ip = attr->value();
            } else if (strcmp(attr->name(), "port") == 0) {
              port = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
            } else if (strcmp(attr->name(), "ioThreadNo") == 0) {
              threadNo = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
            }
          }
        }
      }
    }
  }
  xmlFile.close();
}
const uint16_t ServerConfig::GetPort() const {
  return port;
}
const uint32_t ServerConfig::GetIp() {
  if (ipAddr != 0) {
    return ipAddr;
  }

  SOCKADDR_IN addr{};
  inet_pton(AF_INET, ip.data(), &addr.sin_addr);
  ipAddr = addr.sin_addr.S_un.S_addr;
  return ipAddr;
}
const uint16_t ServerConfig::GetThreadNo() const {
  return threadNo;
}
const uint8_t ServerConfig::GetLogLevel() const {
  return logLevel;
}
const std::string& ServerConfig::GetLogMode() const {
  return logMode;
}
}  // namespace sh::EchoServer
