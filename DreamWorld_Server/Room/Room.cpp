#include "stdafx.h"
#include "Room.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Monster/BossMonsterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include "../MapData/MapData.h"
#include "../GameObject/Character/Warrior/WarriorObject.h"
#include "../GameObject/Character/tanker/TankerObject.h"
#include "../GameObject/Character/Mage/MageObject.h"
#include "../GameObject/Character/Archer/ArcherObject.h"

namespace DreamWorld {
Room::Room(std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
    : m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef) {
}
void Room::Init() {
  static constexpr float SMALL_MONSTER_HP = 250.0f;
  static constexpr float BOSS_HP = 2500.0f;

  std::vector<std::chrono::seconds> duTime;
  std::vector<std::chrono::seconds> coolTime;
  // Character Initialize
  m_characters.emplace(ROLE::WARRIOR, std::make_shared<WarriorObject>(600.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
  m_characters.emplace(ROLE::TANKER, std::make_shared<TankerObject>(780.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
  m_characters.emplace(ROLE::ARCHER, std::make_shared<ArcherObject>(400.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
  m_characters.emplace(ROLE::MAGE, std::make_shared<MageObject>(500.0f, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this())));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::WARRIOR]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::TANKER]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::ARCHER]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::MAGE]));

  auto monsterInitData = m_stageMapData->GetMonsterInitData();
  for (int i = 0; i < monsterInitData.size(); ++i) {
    auto monster = std::make_shared<SmallMonsterObject>(SMALL_MONSTER_HP, 50.0f, 8.0f, std::static_pointer_cast<Room>(shared_from_this()), i);
    monster->SetPosition(monsterInitData[i].position);
    monster->Rotate(ROTATE_AXIS::Y, monsterInitData[i].eulerRotate.y);
    monster->Rotate(ROTATE_AXIS::X, monsterInitData[i].eulerRotate.x);
    monster->Rotate(ROTATE_AXIS::Z, monsterInitData[i].eulerRotate.z);
    InsertGameObject(std::static_pointer_cast<GameObject>(monster));
    m_smallMonsters.push_back(monster);
  }

  m_bossMonster = std::make_shared<BossMonsterObject>(2500.0f, 60.0f, 30.0f, std::static_pointer_cast<Room>(shared_from_this()));
  m_bossMonster->Initialize();
  InsertGameObject(std::static_pointer_cast<GameObject>(m_bossMonster));
  // ProjectileObject Initialize
  for (auto& character : m_characters) {
    character.second->SetStagePosition(m_roomState);
  }
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
