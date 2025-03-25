#pragma once
#include "../PCH/stdafx.h"
#include "../SingletonBase.h"

//class MonsterMapData;
//class NavMapData;
//class Room;
//class UserSession;
//namespace IOCP {
//class Iocp;
//}
//
//class DeprecatedRoomManager : public SingletonBase<DeprecatedRoomManager> {
//  friend SingletonBase;
//
// private:
//  DeprecatedRoomManager() = default;
//  ~DeprecatedRoomManager() = default;
//
// public:
//  void Initialize();
//  void EraseRoom(std::shared_ptr<DeprecatedRoom> roomRef);
//  std::shared_ptr<DeprecatedRoom> MakeRunningRoom(std::vector<std::shared_ptr<UserSession>>& userRefVec);
//  std::shared_ptr<DeprecatedRoom> MakeRunningRoomAloneMode(std::shared_ptr<UserSession>& userRef);
//
// private:
//  std::mutex m_runningRoomLock;
//  std::unordered_set<std::shared_ptr<DeprecatedRoom>> m_runningRooms;
//  std::shared_ptr<MonsterMapData> m_stageMap;
//  std::shared_ptr<NavMapData> m_bossmap;
//
// public:
//  std::atomic<double> globalAvrRoomTick = 0;
//  std::atomic_int globalRoomCnt = 0;
//};
