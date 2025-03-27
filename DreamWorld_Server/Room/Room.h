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
class Session;

class MapData;
class NavMapData;
class MonsterMapData;

class Room
    : public RoomBase {
 public:
  Room(std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef);

  void Init();

  std::vector<std::shared_ptr<LiveObject>> GetMonsters();

  std::vector<std::shared_ptr<LiveObject>> GetCharacters(bool checkAlive = false);

  std::shared_ptr<MapData> GetMapData() const;

  std::shared_ptr<NavMapData> GetBossMapData() const;

  std::shared_ptr<CharacterObject> GetCharacter(const ROLE role);

 protected:
  virtual void Update() override;

 private:
  std::unordered_map<ROLE, std::shared_ptr<CharacterObject>> m_characters;
  // Update를 위한 모든 게임 오브젝트를 담는 vector

  std::vector<std::shared_ptr<SmallMonsterObject>> m_smallMonsters;

  std::shared_ptr<BossMonsterObject> m_bossMonster;
  chrono_clock::time_point m_bossStartTime;

  std::shared_ptr<MonsterMapData> m_stageMapData;
  std::shared_ptr<NavMapData> m_bossMapData;
};
}  // namespace DreamWorld
