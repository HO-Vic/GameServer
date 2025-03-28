#include "stdafx.h"
#include "RoomManager.h"
#include "../LogManager/LogManager.h"
#include <filesystem>
#include "../MapData/MapData.h"

namespace DreamWorld {
RoomManager::RoomManager()
    : m_stageMap(nullptr), m_bossMap(nullptr) {
}
void RoomManager::Init() {
  WRITE_LOG(logLevel::info, "{}({}) > RoomManager Init!", __FUNCTION__, __LINE__);
  m_roomPool.InitSize(300);
  m_stageMap = std::make_shared<MonsterMapData>(std::filesystem::current_path().string().append("\\Data\\StageCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\MonsterStage1.txt"));
  m_bossMap = std::make_shared<NavMapData>(std::filesystem::current_path().string().append("\\Data\\BossCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\BossNavData.txt"));
}

void RoomManager::EraseRoom(std::shared_ptr<Room> roomRef) {
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.erase(reinterpret_cast<uint64_t>(roomRef.get()));
}

std::shared_ptr<Room> RoomManager::MakeRunningRoom(std::vector<std::shared_ptr<Session>>& userRefVec) {
  auto roomPtr = m_roomPool.MakeShared(m_stageMap, m_bossMap);
  roomPtr->Init();
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.emplace(reinterpret_cast<uint64_t>(roomPtr.get()), roomPtr);
  return roomPtr;
}

std::shared_ptr<Room> RoomManager::MakeRunningRoomAloneMode(std::shared_ptr<Session>& userRef) {
  auto roomPtr = m_roomPool.MakeShared(m_stageMap, m_bossMap);
  roomPtr->Init();
  std::lock_guard<std::mutex> lg{m_runningRoomLock};
  m_runningRooms.emplace(reinterpret_cast<uint64_t>(roomPtr.get()), roomPtr);
  return roomPtr;
}
}  // namespace DreamWorld