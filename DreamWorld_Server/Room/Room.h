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

class Room final
    : public RoomBase {
  static constexpr uint8_t GAME_STATE_TICK_COUNT = 3;

 public:
  Room() = default;

  Room(std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef, uint32_t roomId);

  ~Room();

  void Init();

  void StartGame();

  std::vector<std::shared_ptr<LiveObject>> GetMonsters();

  std::vector<std::shared_ptr<LiveObject>> GetCharacters(bool checkAlive = false);

  std::shared_ptr<MapData> GetMapData() const;

  std::shared_ptr<NavMapData> GetBossMapData() const;

  std::shared_ptr<CharacterObject> GetCharacter(const ROLE role);

 protected:
  virtual void Update() override;

 private:
  void SetRoomEndState();

  void SendGameState();

  void CommonStageGameState();

  void BossStageGameState();

 private:
  std::unordered_map<ROLE, std::shared_ptr<CharacterObject>> m_characters;
  // Update를 위한 모든 게임 오브젝트를 담는 vector

  std::vector<std::shared_ptr<SmallMonsterObject>> m_smallMonsters;

  std::shared_ptr<BossMonsterObject> m_bossMonster;
  chrono_clock::time_point m_bossStartTime;

  std::shared_ptr<MonsterMapData> m_stageMapData;
  std::shared_ptr<NavMapData> m_bossMapData;

  chrono_clock::time_point m_prevUpdateTime;
  uint32_t m_roomId;

  uint8_t m_currentUpdateTickCount = 0;
  ROOM_STATE m_roomState = ROOM_STATE::ROOM_COMMON;
};
}  // namespace DreamWorld
