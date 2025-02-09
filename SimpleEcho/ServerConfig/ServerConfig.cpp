#include "pch.h"
#include "ServerConfig.h"
#include <rapidxml/rapidxml.hpp>

namespace sh::EchoServer {
ServerConfig::ServerConfig()
    : ip("127.0.0.1"), port(9000), threadNo(4), ipAddr(0) {
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

  rapidxml::xml_node<char>* root = xmlDoc->first_node();
  for (auto node = root->first_node(); node != nullptr;) {
    for (auto intenalNode = node; intenalNode != nullptr; intenalNode = intenalNode->next_sibling()) {
      auto nodeName = intenalNode->name();
      if (strcmp(nodeName, "Properties") == 0) {
        for (auto attr = intenalNode->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
          if (strcmp(attr->name(), "LogLevel") == 0) {
            uint32_t logLevel = std::stoi(attr->value(), nullptr);
          }
        }
      } else if (strcmp(nodeName, "EchoServer") == 0) {
        for (auto attr = intenalNode->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
          if (strcmp(attr->name(), "ip") == 0) {
            ip = attr->value();
          } else if (strcmp(attr->name(), "port") == 0) {
            port = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
          } else if (strcmp(attr->name(), "threadNo") == 0) {
            threadNo = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
          };
        }
      }
    }
    node = node->next_sibling();
  }

  xmlFile.close();
}
const uint16_t ServerConfig::GetPort() const {
  return port;
}
const uint16_t ServerConfig::GetIp() {
  if (ipAddr != 0) {
    return ipAddr;
  }

  IN_ADDR addr{};
  inet_pton(AF_INET, ip.data(), &addr);
  ipAddr = addr.s_addr;
  return ipAddr;
}
const uint16_t ServerConfig::GetThreadNo() const {
  return threadNo;
}
}  // namespace sh::EchoServer
