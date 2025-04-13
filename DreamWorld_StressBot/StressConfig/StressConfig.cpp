#include "pch.h"
#include "StressConfig.h"
#include <rapidxml/rapidxml.hpp>
#include <inaddr.h>
#include <vector>
#include <fstream>
#include <ws2tcpip.h>

namespace Stress {
Config::Config()
    : ip("127.0.0.1"), port(9000), ioThreadNo(4), ipAddr(0), batchUpdaterThreadNo(2), timerThreadNo(1), batchUpdaterCnt(4), logLevel(1), logMode("ConsoleFile") {
}

void Config::LoadXML(const char* configFile) {
  std::ifstream xmlFile{configFile};
  if (!xmlFile) {
    assert(false &&"invalid xml file");
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
            if (strcmp(node->name(), "Log") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "LogLevel") == 0) {
                  logLevel = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "LogMode") == 0) {
                  logMode = attr->value();
                }
              }
            } else if (strcmp(node->name(), "Threshold") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "DdcUserMaxDelayMs") == 0) {
                  decUserMaxDelayMs = std::stoull(attr->value(), nullptr);
                } else if (strcmp(attr->name(), "adjustConnDelayMs") == 0) {
                  adjustConnDelayMs = std::stoull(attr->value(), nullptr);
                }
              }
            }
          }
        }
      } else if (strcmp(root->name(), "Stress") == 0) {
        if (root->first_node() != nullptr) {
          for (auto node = root->first_node(); node != nullptr; node = node->next_sibling()) {
            if (strcmp(node->name(), "DreamWorldStress") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "ip") == 0) {
                  ip = attr->value();
                } else if (strcmp(attr->name(), "port") == 0) {
                  port = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "ioThreadNo") == 0) {
                  ioThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "batchUpdaterThreadNo") == 0) {
                  batchUpdaterThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "timerThreadNo") == 0) {
                  timerThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "batchUpdaterCnt") == 0) {
                  batchUpdaterCnt = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                }
              }
            }
          }
        }
      }
    }
  }
  xmlFile.close();
}

const uint32_t Config::GetServerIp() {
  if (ipAddr != 0) {
    return ipAddr;
  }

  SOCKADDR_IN addr{};
  inet_pton(AF_INET, ip.data(), &addr.sin_addr);
  ipAddr = addr.sin_addr.S_un.S_addr;
  return ipAddr;
}
}  // namespace Stress