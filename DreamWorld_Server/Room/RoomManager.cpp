#include "stdafx.h"
#include "RoomManager.h"
#include "../LogManager/LogManager.h"
#include <filesystem>
#include "../MapData/MapData.h"
#include "Room.h"
#include "../Server/ServerConfig.h"

namespace DreamWorld {
RoomManager::RoomManager()
    : m_stageMap(nullptr), m_bossMap(nullptr), m_roomId(0), m_isAbleMakeRoom(true) {
}
void RoomManager::Init() {
  WRITE_LOG(logLevel::info, "{}({}) > RoomManager Init!", __FUNCTION__, __LINE__);
  m_roomPool.InitSize(300);
  m_stageMap = std::make_shared<MonsterMapData>(std::filesystem::current_path().string().append("\\Data\\StageCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\MonsterStage1.txt"));
  m_bossMap = std::make_shared<NavMapData>(std::filesystem::current_path().string().append("\\Data\\BossCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\BossNavData.txt"));
  m_prevLoggingTime = chrono_clock::now();
  m_prevMaxRoomLoggingTime = chrono_clock::now();
}

void RoomManager::EraseRoom(std::shared_ptr<Room> roomRef) {
  auto roomId = roomRef->GetId();
  m_freeRoomIds.push(roomId);
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.erase(reinterpret_cast<uint64_t>(roomRef.get()));
}

std::shared_ptr<Room> RoomManager::MakeRunningRoom(std::vector<std::shared_ptr<Session>>& userRefVec) {
  uint64_t globalTick = globalAvgRoomTick;
  uint32_t roomCnt = globalRoomCnt;
  if (0 != roomCnt) {
    globalTick = globalTick / roomCnt;
    if (!m_isAbleMakeRoom) {
      if (globalTick < ServerConfig::GetInstance().reactiveTickMs) {
        WRITE_LOG(logLevel::warn, "{}({}) > Reactive Create Room!! [GlobalAvgTick:{}Ms] [Room Count:{}]", __FUNCTION__, __LINE__, globalTick, roomCnt);
        m_prevMaxRoomLoggingTime = m_prevMaxRoomLoggingTime - SEC(100);
        m_isAbleMakeRoom = true;
      }
    }
    if (globalTick > ServerConfig::GetInstance().maxRoomTickMs) {
      auto loggingDiff = _chrono::duration_cast<_chrono::milliseconds>(chrono_clock::now() - m_prevMaxRoomLoggingTime).count();
      if (loggingDiff > 2000) {
        WRITE_LOG(logLevel::warn, "{}({}) > Limit Create Room!! [GlobalAvgTick:{}Ms] [Room Count:{}]", __FUNCTION__, __LINE__, globalTick, roomCnt);
        m_prevMaxRoomLoggingTime = chrono_clock::now();
      }
      m_isAbleMakeRoom = false;
      return nullptr;
    }
  }
  uint32_t roomId = 0;
  m_freeRoomIds.try_pop(roomId);
  if (roomId == 0) {
    roomId = ++m_roomId;
  }
  auto roomPtr = m_roomPool.MakeShared(m_stageMap, m_bossMap, roomId);
  roomPtr->Init();
  globalRoomCnt++;
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.emplace(reinterpret_cast<uint64_t>(roomPtr.get()), roomPtr);
  return roomPtr;
}

std::shared_ptr<Room> RoomManager::MakeRunningRoomAloneMode(std::shared_ptr<Session>& userRef) {  // 이건 테스트할 때 씀
  uint32_t roomId = 0;
  m_freeRoomIds.try_pop(roomId);
  if (roomId == 0) {
    roomId = ++m_roomId;
  }
  auto roomPtr = m_roomPool.MakeShared(m_stageMap, m_bossMap, roomId);
  roomPtr->Init();
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.emplace(reinterpret_cast<uint64_t>(roomPtr.get()), roomPtr);
  return roomPtr;
}
}  // namespace DreamWorld