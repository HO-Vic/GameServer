#pragma once
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <Utility/Thread/IWorkerItem.h>
#include "RoomBase.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace DreamWorld {
#define _chrono std::chrono;
class SmallMonsterObject;
class BossMonsterObject;
class MonsterMapData;
class NavMapData;
class Session;

enum class ROOM_STATE : char {
  ROOM_COMMON,
  ROOM_BOSS,
  ROOM_END
};

class Room
    : public RoomBase {
 public:

 protected:
  void Update();

 private:
  
};
}  // namespace DreamWorld
