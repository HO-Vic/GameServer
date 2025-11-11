#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <shared_mutex>
#include <Utility/Thread/IWorkerItem.h>
#include "../JobQ/LFJobQ.h"
#include <Utility/Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>
#include <Utility/Job/JobQueue/JobQ_MT/JobQ_MT.h>
#include "../DreamWorldDefine.h"
#include "../Network/Session/Session.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace DreamWorld {
using JobQ = sh::Utility::DoubleJobQ_MT;
// using JobQ = sh::Utility::JobQ_MT;
// using JobQ = LFJobQ;

class SmallMonsterObject;
class BossMonsterObject;
class ProjectileObject;
class CharacterObject;
class GameObject;
class MonsterMapData;
class NavMapData;
class Session;

struct PacketHeader;

class RoomBase
    : public sh::Utility::IWorkerItem,
      public sh::Utility::DoubleJobQ_MT {
 public:
  RoomBase(const uint16_t maxExecuteJobCnt = 0);

  virtual bool Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

  // InsertPlayer, DiscardPlayer는 Room생성 시 1회, JobQ로 호출되기때문에, 싱글 쓰레드로 동작
  bool InsertPlayer(std::shared_ptr<Session>& player);

  void DiscardPlayer(std::shared_ptr<Session> player);

  template <class T>
  void Broadcast(const T* packet, std::shared_ptr<Session> ignore = nullptr) {
    std::shared_lock<std::shared_mutex> lg{m_userLock};
    for (auto& [uniqueNo, sessionPtr] : m_Sessions) {
      if (sessionPtr != ignore) {
        sessionPtr->DoSend(packet);
      }
    }
  }

  template <class T>
  void Broadcast(const T* packet, std::vector<std::shared_ptr<Session>> ignorePlayers) {
    std::shared_lock<std::shared_mutex> lg{m_userLock};
    if (m_Sessions.empty()) {
      return;
    }

    for (auto& [uniqueNo, sessionPtr] : m_Sessions) {
      if (!ignorePlayers.empty()) {
        auto ignorePlayerIter = std::find_if(ignorePlayers.begin(), ignorePlayers.end(), [uniqueNo](const std::shared_ptr<Session>& ignorePlayer) {
          return uniqueNo == ignorePlayer->GetUniqueNo();
        });
        if (ignorePlayerIter != ignorePlayers.end()) {
          std::swap(*ignorePlayerIter, ignorePlayers.back());
          ignorePlayers.pop_back();
          continue;
        }
      }
      sessionPtr->DoSend(packet);
    }
  }

  template <class T>
  void Broadcast(const T* packet, const std::unordered_set<uint32_t>& ignoreUniqueNos) {
    std::shared_lock<std::shared_mutex> lg{m_userLock};
    for (auto& [uniqueNo, sessionPtr] : m_Sessions) {
      if (!ignoreUniqueNos.contains(uniqueNo)) {
        sessionPtr->DoSend(packet);
      }
    }
  }

  virtual std::vector<std::shared_ptr<GameObject>> GetLiveObjects();

  void InsertProjectileObject(std::shared_ptr<ProjectileObject>& projectileObject);

 protected:
  virtual void Update();

  void InsertGameObject(std::shared_ptr<GameObject>& gameObject);

  std::vector<std::shared_ptr<Session>> GetUserSession();

  size_t GetActiveUser();

  void IntenalUpdateGameObjet();

  void IntenalUpdateProjectileObject();

 private:
  int m_updateCnt;
  uint16_t m_maxExecJobCnt;

  // 플레이어 id-캐릭터
  // 패킷을 보내는건 2개 이상의 쓰레드에서 하는 경우가 많은데
  // read만 하는데도 mutex는 안좋은 방법이라고 판단-> read lock으로 변경, 유저가 들어올 때, 나갈때는 write-lock을 할 의도
  std::shared_mutex m_userLock;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> m_Sessions;

  // 투사체는 사라질 수 있으니 list로 처리
  std::list<std::shared_ptr<ProjectileObject>> m_projectileObjects;
  // 업데이트할 게임 오브젝트들
  std::vector<std::shared_ptr<GameObject>> m_gameObjects;

  // std::chrono::high_resolution_clock::time_point prevUpdateTime;
};
}  // namespace DreamWorld
