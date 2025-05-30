#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <cstdint>
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include "Room.h"

namespace DreamWorld {
class MonsterMapData;
class NavMapData;
class Session;

class RoomManager
    : public sh::Utility::SingletonBase<RoomManager> {
 public:
  RoomManager();

  void Init();

  void EraseRoom(std::shared_ptr<Room> roomRef);

  std::shared_ptr<Room> MakeRunningRoom(std::vector<std::shared_ptr<Session>>& userRefVec);

  std::shared_ptr<Room> MakeRunningRoomAloneMode(std::shared_ptr<Session>& userRef);

  sh::Utility::ObjectPool<Room>& GetRoomPool() {
    return m_roomPool;
  }

 private:
  std::mutex m_runningRoomLock;
  std::unordered_map<uint64_t, std::shared_ptr<Room>> m_runningRooms;
  std::shared_ptr<MonsterMapData> m_stageMap;
  std::shared_ptr<NavMapData> m_bossMap;
  sh::Utility::ObjectPool<Room> m_roomPool;
  std::atomic<uint32_t> m_roomId;
  tbb::concurrent_queue<uint32_t> m_freeRoomIds;

  bool m_isAbleMakeRoom;

 public:
  // RoomMetric
  std::atomic_llong globalAvgRoomTick = 0;  // Ms
  std::atomic_uint globalRoomCnt = 0;

  chrono_clock::time_point m_prevLoggingTime;
  chrono_clock::time_point m_prevMaxRoomLoggingTime;
};
}  // namespace DreamWorld
