#pragma once
#include <chrono>
#include <unordered_map>
#include "RoomBase.h"
#include "../DreamWorldDefine.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace DreamWorld {

class SmallMonsterObject;
class BossMonsterObject;
class MonsterMapData;
class LiveObject;
class NavMapData;
class Session;

class Room
    : public RoomBase {
 public:
  Room();

  virtual void Init();

  std::vector<std::shared_ptr<LiveObject>> GetMonsters();

  std::vector<std::shared_ptr<LiveObject>> GetCharacters(bool checkAlive = false);

 protected:
  virtual void Update() override;

 private:
  std::unordered_map<ROLE, std::shared_ptr<CharacterObject>> m_characters;
  // Update를 위한 모든 게임 오브젝트를 담는 vector

  std::vector<std::shared_ptr<SmallMonsterObject>> m_smallMonsters;

  std::shared_ptr<BossMonsterObject> m_bossMonster;
  chrono_clock::time_point m_bossStartTime;
};
}  // namespace DreamWorld
