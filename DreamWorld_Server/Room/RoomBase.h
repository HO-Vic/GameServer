#pragma once
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_set>
#include <list>
#include <shared_mutex>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Job/JobQueue/JobQ_MT/LFJobQ_MT.h>
#include "../DreamWorldDefine.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace DreamWorld {
#define _chrono std::chrono;
class SmallMonsterObject;
class BossMonsterObject;
class ProjectileObject;
class CharacterObject;
class GameObject;
class MonsterMapData;
class NavMapData;
class Session;

class PacketHeader;

class RoomBase
    : public sh::Utility::IWorkerItem,
      public sh::Utility::LFJobQ_MT {
 public:
  RoomBase(const uint16_t maxExecuteJobCnt = 0);

  virtual void Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte) override;

  bool InsertPlayer(Session* caster);

  void DiscardPlayer(Session* caster);

  void Broadcast(PacketHeader*, Session* ignore = nullptr);

  void Broadcast(PacketHeader*, const std::vector<std::shared_ptr<Session>>& ignorePlayers);

 protected:
  virtual void Update();

  void InsertProjectileObject(std::shared_ptr<ProjectileObject>&& projectileObject);

  void InsertGameObject(std::shared_ptr<GameObject>&& gameObject);

 private:
  void IntenalUpdateGameObjet();

  void IntenalUpdateProjectileObject();

 private:
  ROOM_STATE m_roomState = ROOM_STATE::ROOM_COMMON;
  int m_updateCnt;
  uint16_t m_maxExecJobCnt;

  // 플레이어 id-캐릭터
  // 패킷을 보내는건 2개 이상의 쓰레드에서 하는 경우가 많은데
  // read만 하는데도 mutex는 안좋은 방법이라고 판단-> read lock으로 변경, 유저가 들어올 때, 나갈때는 write-lock을 할 의도
  std::shared_mutex m_userLock;
  std::unordered_set<std::shared_ptr<Session>> m_Sessions;

  // 투사체는 사라질 수 있으니 list로 처리
  std::list<std::shared_ptr<ProjectileObject>> m_projectileObjects;
  // 업데이트할 게임 오브젝트들
  std::vector<std::shared_ptr<GameObject>> m_gameObjects;

  // std::chrono::high_resolution_clock::time_point prevUpdateTime;
};
}  // namespace DreamWorld
