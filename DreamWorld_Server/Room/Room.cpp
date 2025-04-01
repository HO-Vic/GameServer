#include "stdafx.h"
#include "Room.h"
#include <thread>
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
#include "../Server/MsgProtocol.h"
#include "../Network/Session/Session.h"
#include "../Timer/Timer.h"
#include "../Timer/TimerJob.h"
#include "../ObjectPools.h"
#include "RoomThreadPool.h"
#include "RoomManager.h"
#include "../LogManager/LogManager.h"
#include "../Server/ServerConfig.h"

namespace DreamWorld {
Room::Room(std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef, uint32_t roomId)
    : m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef), m_roomId(roomId) {
}

Room::~Room() {
  WRITE_LOG(logLevel::trace, "{}({}) > destructor Room", __FUNCTION__, __LINE__);
}

void Room::Init() {
  static constexpr float SMALL_MONSTER_HP = 250.0f;
  static constexpr float BOSS_HP = 2500.0f;

  std::vector<std::chrono::seconds> duTime;
  std::vector<std::chrono::seconds> coolTime;
  // Character Initialize
  m_characters.emplace(ROLE::WARRIOR, std::make_shared<WarriorObject>(600.0f, 50.0f, 8.0f, std::static_pointer_cast<RoomBase>(shared_from_this())));
  m_characters.emplace(ROLE::TANKER, std::make_shared<TankerObject>(780.0f, 50.0f, 8.0f, std::static_pointer_cast<RoomBase>(shared_from_this())));
  m_characters.emplace(ROLE::ARCHER, std::make_shared<ArcherObject>(400.0f, 50.0f, 8.0f, std::static_pointer_cast<RoomBase>(shared_from_this())));
  m_characters.emplace(ROLE::MAGE, std::make_shared<MageObject>(500.0f, 50.0f, 8.0f, std::static_pointer_cast<RoomBase>(shared_from_this())));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::WARRIOR]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::TANKER]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::ARCHER]));
  InsertGameObject(std::static_pointer_cast<GameObject>(m_characters[ROLE::MAGE]));

  auto monsterInitData = m_stageMapData->GetMonsterInitData();
  for (int i = 0; i < monsterInitData.size(); ++i) {
    auto monster = std::make_shared<SmallMonsterObject>(SMALL_MONSTER_HP, 50.0f, 8.0f, std::static_pointer_cast<RoomBase>(shared_from_this()), i);
    monster->SetPosition(monsterInitData[i].position);
    monster->Rotate(ROTATE_AXIS::Y, monsterInitData[i].eulerRotate.y);
    monster->Rotate(ROTATE_AXIS::X, monsterInitData[i].eulerRotate.x);
    monster->Rotate(ROTATE_AXIS::Z, monsterInitData[i].eulerRotate.z);
    InsertGameObject(std::static_pointer_cast<GameObject>(monster));
    m_smallMonsters.push_back(monster);
  }

  m_bossMonster = std::make_shared<BossMonsterObject>(2500.0f, 60.0f, 30.0f, std::static_pointer_cast<RoomBase>(shared_from_this()));
  m_bossMonster->Initialize();
  InsertGameObject(std::static_pointer_cast<GameObject>(m_bossMonster));
  // ProjectileObject Initialize
  for (auto& character : m_characters) {
    character.second->SetStagePosition(m_roomState);
  }
  m_prevUpdateTime = chrono_clock::now();
  m_prevLogTime = m_prevUpdateTime;
}

void Room::StartGame() {
  DreamWorld::SERVER_PACKET::IntoGamePacket sendPacket{};
  for (int i = 0; i < 15; ++i) {
    sendPacket.monsterData[i].id = i;
    sendPacket.monsterData[i].position = m_smallMonsters[i]->GetPosition();
    sendPacket.monsterData[i].lookVector = m_smallMonsters[i]->GetLookVector();
    sendPacket.monsterData[i].maxHp = m_smallMonsters[i]->GetMaxHp();
  }

  for (auto& session : GetUserSession()) {
    sendPacket.role = session->GetIngameRole();
    session->DoSend(&sendPacket, sendPacket.size);
  };

  Timer::GetInstance().InsertTimerEvent(
      ObjectPool<TimerJob>::GetInstance().MakeUnique(chrono_clock::now() + ROOM_UPDATE_TICK, std::move([=]() {
                                                       RoomThreadPool::GetInstance().InsertRoomUpdateEvent(
                                                           std::static_pointer_cast<RoomBase>(shared_from_this()));
                                                     })));
}

std::vector<std::shared_ptr<LiveObject>> Room::GetMonsters() {
  std::vector<std::shared_ptr<LiveObject>> smallMonsters;
  smallMonsters.reserve(15);
  for (auto& monster : m_smallMonsters) {
    smallMonsters.push_back(monster);
  }
  return smallMonsters;
}

std::vector<std::shared_ptr<LiveObject>> Room::GetCharacters(bool checkAlive) {
  std::vector<std::shared_ptr<LiveObject>> liveCaharacters;
  liveCaharacters.reserve(4);
  for (auto& [role, character] : m_characters) {
    if (checkAlive) {
      if (!character->IsAlive()) {
        continue;
      }
    }
    liveCaharacters.push_back(character);
  }
  return liveCaharacters;
}

void Room::Update() {
  DoJobs(0);
  if (ROOM_STATE::ROOM_END == m_roomState) {
    return;
  }

  int aliveCharacterCnt = 0;
  for (auto& [role, character] : m_characters) {
    character->Update();
    if (character->IsAlive()) {
      ++aliveCharacterCnt;
    }
  }

  if (aliveCharacterCnt == 0) {  // 모든 플레이어가 죽어도 게임 종료
    SetRoomEndState();
    return;
  }

  if (ROOM_STATE::ROOM_COMMON == m_roomState) {
    for (auto& smallMonster : m_smallMonsters) {
      smallMonster->Update();
    }
  } else if (ROOM_STATE::ROOM_BOSS == m_roomState) {
    if (m_bossStartTime < std::chrono::high_resolution_clock::now()) {
      m_bossMonster->Update();
    }
  }

  IntenalUpdateProjectileObject();

  if (!m_bossMonster->IsAlive()) {  // 게임 종료 조건
    SetRoomEndState();
    return;
  }

  if (GAME_STATE_TICK_COUNT == m_currentUpdateTickCount++) {
    if (GetActiveUser() == 0) {
      SetRoomEndState();
      return;
    }
    SendGameState();
    m_currentUpdateTickCount = 0;
  }
  auto now = chrono_clock::now();
  auto tDiff = _chrono::duration_cast<_chrono::milliseconds>(now - m_prevUpdateTime);

  auto logTimeDiff = _chrono::duration_cast<_chrono::milliseconds>(now - m_prevLogTime);
  bool isLogging = ServerConfig::GetInstance().logTickSec <= logTimeDiff;
  Timer::GetInstance().InsertTimerEvent(
      ObjectPool<TimerJob>::GetInstance().MakeUnique(now + ROOM_UPDATE_TICK, std::move([isLogging, now, this]() {
                                                       RoomThreadPool::GetInstance().InsertRoomUpdateEvent(std::static_pointer_cast<RoomBase>(shared_from_this()));
                                                       /*if (isLogging && ServerConfig::GetInstance().targetRoomIds.contains(m_roomId)) {
                                                         auto execTime = chrono_clock::now();
                                                         auto diff = _chrono::duration_cast<_chrono::milliseconds>(execTime - now).count();
                                                         WRITE_LOG(logLevel::debug, "{}({}) > Room Update Time Check Diff Insert to Execute [roomId:{}][Diff:{}] [tick: {}]]", __FUNCTION__, __LINE__, m_roomId, diff, ROOM_UPDATE_TICK.count());
                                                       }*/
                                                     })));

  m_prevUpdateTime = now;
  if (isLogging && ServerConfig::GetInstance().targetRoomIds.contains(m_roomId)) {
    m_prevLogTime = now;
    size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());

    WRITE_LOG(logLevel::debug, "{}({}) > Room Update Tick Diff {}ms [roomId:{}][thId:{}]", __FUNCTION__, __LINE__, tDiff.count(), m_roomId, threadId);
  }
}

void Room::SetRoomEndState() {
  m_roomState = ROOM_STATE::ROOM_END;
  SERVER_PACKET::NotifyPacket sendPacket(static_cast<char>(SERVER_PACKET::TYPE::GAME_END));
  Broadcast(&sendPacket, nullptr);
  RoomManager::GetInstance().EraseRoom(std::static_pointer_cast<Room>(shared_from_this()));
}

void Room::SendGameState() {
  if (ROOM_STATE::ROOM_COMMON == m_roomState) {
    CommonStageGameState();
    return;
  }
  BossStageGameState();
}

void Room::CommonStageGameState() {
  auto gameStatePacket = SERVER_PACKET::GameState_STAGE{};
  int characterIdx = 0;
  for (auto& [role, character] : m_characters) {
    gameStatePacket.userState[characterIdx].role = role;
    gameStatePacket.userState[characterIdx].hp = character->GetHp();
    gameStatePacket.userState[characterIdx].position = character->GetPosition();
    gameStatePacket.userState[characterIdx].resetShield = character->GetShield();
    gameStatePacket.userState[characterIdx].time = character->GetLastUpdateTime();
    ++characterIdx;
  }
  for (auto& monster : m_smallMonsters) {
    gameStatePacket.smallMonster[monster->GetIdx()].isAlive = monster->IsAlive();
    gameStatePacket.smallMonster[monster->GetIdx()].idx = monster->GetIdx();
    gameStatePacket.smallMonster[monster->GetIdx()].hp = monster->GetHp();
    gameStatePacket.smallMonster[monster->GetIdx()].position = monster->GetPosition();
    gameStatePacket.smallMonster[monster->GetIdx()].time = monster->GetLastUpdateTime();
  }
  Broadcast(&gameStatePacket);
}

void Room::BossStageGameState() {
  auto gameStatePacket = SERVER_PACKET::GameState_BOSS{};
  int characterIdx = 0;
  for (auto& [role, character] : m_characters) {
    gameStatePacket.userState[characterIdx].role = role;
    gameStatePacket.userState[characterIdx].hp = character->GetHp();
    gameStatePacket.userState[characterIdx].position = character->GetPosition();
    gameStatePacket.userState[characterIdx].resetShield = character->GetShield();
    gameStatePacket.userState[characterIdx].time = character->GetLastUpdateTime();
    ++characterIdx;
  }
  gameStatePacket.bossState.hp = m_bossMonster->GetHp();
  gameStatePacket.bossState.position = m_bossMonster->GetPosition();
  gameStatePacket.bossState.time = m_bossMonster->GetLastUpdateTime();
  Broadcast(&gameStatePacket);
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

std::shared_ptr<CharacterObject> Room::GetCharacter(const ROLE role) {
  auto findIter = m_characters.find(role);
  if (findIter != m_characters.end()) {
    return findIter->second;
  }
  return nullptr;
}

void Room::SetBossStage() {
  static constexpr std::chrono::seconds BOSS_START_AFTER_TIME = std::chrono::seconds(11);
  if (ROOM_STATE::ROOM_COMMON != m_roomState) {
    return;
  }
  m_roomState = ROOM_STATE::ROOM_BOSS;
  m_bossStartTime = std::chrono::high_resolution_clock::now() + BOSS_START_AFTER_TIME;

  SERVER_PACKET::BossStageInitPacket sendPacket{};
  int characterCnt = 0;
  for (auto& [role, character] : m_characters) {
    character->ForceStopMove();
    character->SetStagePosition(ROOM_STATE::ROOM_BOSS);
    character->ResetSkillCoolTime();
    character->SetFullHp();
    sendPacket.userState[characterCnt].role = role;
    sendPacket.userState[characterCnt].hp = character->GetHp();
    sendPacket.userState[characterCnt].position = character->GetPosition();
    sendPacket.userState[characterCnt].resetShield = character->GetShield();
    ++characterCnt;
  }
  sendPacket.bossPosition = m_bossMonster->GetPosition();
  sendPacket.bossLookVector = m_bossMonster->GetLookVector();
  sendPacket.bossHp = m_bossMonster->GetMaxHp();
  Broadcast(&sendPacket);
}

void Room::ForceGameEnd() {
  if (ROOM_STATE::ROOM_END != m_roomState) {
    SetRoomEndState();
  }
}
}  // namespace DreamWorld
