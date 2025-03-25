#include "stdafx.h"
#include "Room.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Monster/MonsterObject.h"
#include "../GameObject/Monster/SmallMonsterObject.h"
#include "../GameObject/Monster/BossMonsterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"

namespace DreamWorld {
std::vector<std::shared_ptr<LiveObject>> Room::GetMonsters() {
  return std::vector<std::shared_ptr<LiveObject>>();
}

std::vector<std::shared_ptr<LiveObject>> Room::GetCharacters(bool checkAlive) {
  return std::vector<std::shared_ptr<LiveObject>>();
}

void Room::Update() {
}
}  // namespace DreamWorld
