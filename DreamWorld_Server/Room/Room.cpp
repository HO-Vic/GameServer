#include "stdafx.h"
#include "Room.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Monster/BossMonsterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include "../MapData/MapData.h"

namespace DreamWorld {
Room::Room(std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
    : m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef) {
}
void Room::Init() {
}
std::vector<std::shared_ptr<LiveObject>> Room::GetMonsters() {
  return std::vector<std::shared_ptr<LiveObject>>();
}

std::vector<std::shared_ptr<LiveObject>> Room::GetCharacters(bool checkAlive) {
  return std::vector<std::shared_ptr<LiveObject>>();
}

void Room::Update() {
}

std::shared_ptr<MapData> Room::GetMapData() const {
  if (ROOM_STATE::ROOM_COMMON == m_roomState) {
    return m_stageMapData;
  }
  return m_bossMapData;
}

std::shared_ptr<NavMapData> Room::GetBossMapData() const {
  return m_bossMapData;
}
}  // namespace DreamWorld
