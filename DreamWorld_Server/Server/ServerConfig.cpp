#include "stdafx.h"
#include "ServerConfig.h"
#include <rapidxml/rapidxml.hpp>
#include <inaddr.h>
#include <vector>
#include <fstream>
#include <ws2tcpip.h>

namespace DreamWorld {
ServerConfig::ServerConfig()
    : ip("0.0.0.0"), port(9000), ioThreadNo(4), ipAddr(0), DBThreadNo(2), roomThreadNo(4), timerThreadNo(1), logLevel(1), targetRoomLogTickSec(1), avgRoomLogTickSec(1), maxRoomTickMs(120), reactiveTickMs(100) {
}

void ServerConfig::LoadXML(const char* configFile) {
  std::ifstream xmlFile{configFile};
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
            if (strcmp(node->name(), "Log") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "LogLevel") == 0) {
                  logLevel = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "LogMode") == 0) {
                  logMode = attr->value();
                }
              }
            } else if (strcmp(node->name(), "RoomMetric") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "targetRoomId") == 0) {
                  std::string roomIds = attr->value();
                  size_t findOffset = -1;
                  while (true) {
                    findOffset = roomIds.find(',', findOffset + 1);
                    if (std::string::npos == findOffset) {
                      break;
                    }
                    targetRoomIds.insert(std::stoi(&roomIds[findOffset + 1]));
                  }
                } else if (strcmp(attr->name(), "logTickSec") == 0) {
                  targetRoomLogTickSec = DreamWorld::SEC(static_cast<uint16_t>(std::stoi(attr->value(), nullptr)));
                }
              }
            } else if (strcmp(node->name(), "RoomAvgMetric") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "logTickSec") == 0) {
                  avgRoomLogTickSec = DreamWorld::SEC(static_cast<uint16_t>(std::stoi(attr->value(), nullptr)));
                }
              }
            } else if (strcmp(node->name(), "RoomAvgDelayTick") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "MaxTickMs") == 0) {
                  maxRoomTickMs = static_cast<uint32_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "ReActiveTickMs") == 0) {
                  reactiveTickMs = static_cast<uint32_t>(std::stoi(attr->value(), nullptr));
                }
              }
            } else if (strcmp(node->name(), "Metric") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "IO_Metric") == 0) {
                  useIOMetric = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "Game_Metric") == 0) {
                  useGameMetric = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "logTickSec") == 0) {
                  meticLoggingTickSec = DreamWorld::SEC(static_cast<uint16_t>(std::stoi(attr->value(), nullptr)));
                }
              }
            }
          }
        }
      } else if (strcmp(root->name(), "Server") == 0) {
        if (root->first_node() != nullptr) {
          for (auto node = root->first_node(); node != nullptr; node = node->next_sibling()) {
            if (strcmp(node->name(), "DreamWorldServer") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "ip") == 0) {
                  ip = attr->value();
                } else if (strcmp(attr->name(), "port") == 0) {
                  port = static_cast<uint16_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "ioThreadNo") == 0) {
                  ioThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "roomThreadNo") == 0) {
                  roomThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "timerThreadNo") == 0) {
                  timerThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                } else if (strcmp(attr->name(), "DBThreadNo") == 0) {
                  DBThreadNo = static_cast<uint8_t>(std::stoi(attr->value(), nullptr));
                }
              }
            }
          }
        }
      } else if (strcmp(root->name(), "DB") == 0) {
        if (root->first_node() != nullptr) {
          for (auto node = root->first_node(); node != nullptr; node = node->next_sibling()) {
            if (strcmp(node->name(), "UserDB") == 0) {
              for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "ip") == 0) {
                  dbIp = attr->value();
                } else if (strcmp(attr->name(), "port") == 0) {
                  dbPort = attr->value();
                } else if (strcmp(attr->name(), "name") == 0) {
                  dbName = attr->value();
                } else if (strcmp(attr->name(), "id") == 0) {
                  dbId = attr->value();
                } else if (strcmp(attr->name(), "pw") == 0) {
                  dbpw = attr->value();
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

const uint32_t ServerConfig::GetServerIp() {
  if (ipAddr != 0) {
    return ipAddr;
  }

  SOCKADDR_IN addr{};
  inet_pton(AF_INET, ip.data(), &addr.sin_addr);
  ipAddr = addr.sin_addr.S_un.S_addr;
  return ipAddr;
}
}  // namespace DreamWorld