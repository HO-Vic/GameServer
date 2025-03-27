#include "stdafx.h"
#include "RoomManager.h"

namespace DreamWorld {
void RoomManager::Init() {
  m_roomPool.InitSize(300);
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