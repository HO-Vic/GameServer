#include "stdafx.h"
#include "DeprecatedRoomManager.h"
#include "DeprecatedRoom.h"
#include "../MapData/MapData.h"

//void DeprecatedRoomManager::Initialize() {
//  m_stageMap = std::make_shared<MonsterMapData>(std::filesystem::current_path().string().append("\\Data\\StageCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\MonsterStage1.txt"));
//  m_bossmap = std::make_shared<NavMapData>(std::filesystem::current_path().string().append("\\Data\\BossCollisionData.txt"), std::filesystem::current_path().string().append("\\Data\\BossNavData.txt"));
//}
//
//void DeprecatedRoomManager::EraseRoom(std::shared_ptr<DeprecatedRoom> roomRef) {
//  std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
//  m_runningRooms.erase(roomRef);
//  globalRoomCnt--;
//}
//
//std::shared_ptr<DeprecatedRoom> DeprecatedRoomManager::MakeRunningRoom(std::vector<std::shared_ptr<UserSession>>& userRefVec) {
//  auto roomRef = std::make_shared<DeprecatedRoom>(userRefVec, m_stageMap, m_bossmap);
//  std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
//  m_runningRooms.insert(roomRef);
//  globalRoomCnt++;
//  return roomRef;
//}
//
//std::shared_ptr<DeprecatedRoom> DeprecatedRoomManager::MakeRunningRoomAloneMode(std::shared_ptr<UserSession>& userRef) {
//  auto roomRef = std::make_shared<DeprecatedRoom>(userRef, m_stageMap, m_bossmap);
//  std::lock_guard<std::mutex> runningLg(m_runningRoomLock);
//  m_runningRooms.insert(roomRef);
//  globalRoomCnt++;
//  return roomRef;
//}
