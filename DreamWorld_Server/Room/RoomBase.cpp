#include "stdafx.h"
#include "RoomBase.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"
#include "../Network/Session/Session.h"
#include "../LogManager/LogManager.h"
#include "../Server/MsgProtocol.h"
#include "../Timer/Timer.h"
#include "../Timer/TimerJob.h"
#include "RoomThreadPool.h"
#include "../ObjectPools.h"
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include "../Metric/Metric.h"

namespace DreamWorld {
RoomBase::RoomBase(const uint16_t maxExecuteJobCnt)
    : sh::Utility::DoubleJobQ_MT("RoomBase"), m_maxExecJobCnt(maxExecuteJobCnt) {
}

void RoomBase::Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte) {
  Update();
  MetricSlot::GetInstance().RecordRoom();
  sh::IO_Engine::ThWorkerJobPool::GetInstance().Release(workerJob);
}

bool RoomBase::InsertPlayer(std::shared_ptr<Session>& player) {
  std::lock_guard<std::shared_mutex> lg{m_userLock};
  if (m_Sessions.size() > 3) {
    return false;
  }
  if (m_Sessions.contains(player->GetUniqueNo())) {
    WRITE_LOG(logLevel::err, "{}({}) > Already Exist User!! [RoomNo:{}] [userId:{}]", __FUNCTION__, __LINE__, 0, 11);
    return false;
  }
  m_Sessions.emplace(player->GetUniqueNo(), player);
  return true;
}

void RoomBase::DiscardPlayer(std::shared_ptr<Session> player) {
  std::lock_guard<std::shared_mutex> lg{m_userLock};
  if (!m_Sessions.contains(player->GetUniqueNo())) {
    WRITE_LOG(logLevel::err, "{}({}) > Non Exist User!! [RoomNo:{}] [userId:{}]", __FUNCTION__, __LINE__, 0, 11);
    return;
  }
  m_Sessions.erase(player->GetUniqueNo());
}

void RoomBase::Broadcast(PacketHeader* sendPacket, std::shared_ptr<Session> ignore /*= nullptr*/) {
  std::shared_lock<std::shared_mutex> lg{m_userLock};
  for (auto& [uniqueNo, sessionPtr] : m_Sessions) {
    if (sessionPtr != ignore) {
      sessionPtr->DoSend(sendPacket, sendPacket->size);
    }
  }
}

void RoomBase::Broadcast(PacketHeader* sendPacket, std::vector<std::shared_ptr<Session>> ignorePlayers) {
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
    sessionPtr->DoSend(sendPacket, sendPacket->size);
  }
}

void RoomBase::Broadcast(PacketHeader* sendPacket, const std::unordered_set<uint32_t>& ignoreUniqueNos) {
  std::shared_lock<std::shared_mutex> lg{m_userLock};
  for (auto& [uniqueNo, sessionPtr] : m_Sessions) {
    if (!ignoreUniqueNos.contains(uniqueNo)) {
      sessionPtr->DoSend(sendPacket, sendPacket->size);
    }
  }
}

std::vector<std::shared_ptr<GameObject>> RoomBase::GetLiveObjects() {
  return m_gameObjects;
}

void RoomBase::Update() {
  DoJobs(0);  // 몇이 좋을지는 모르겠음
  IntenalUpdateGameObjet();
  IntenalUpdateProjectileObject();
}

void RoomBase::InsertProjectileObject(std::shared_ptr<ProjectileObject>& projectileObject) {
  m_projectileObjects.push_back(projectileObject);
}

void RoomBase::InsertGameObject(std::shared_ptr<GameObject>& gameObject) {
  m_gameObjects.push_back(gameObject);
}

std::vector<std::shared_ptr<Session>> RoomBase::GetUserSession() {
  std::vector<std::shared_ptr<Session>> returnUsers;
  returnUsers.reserve(4);
  std::shared_lock lg{m_userLock};
  for (auto& [role, session] : m_Sessions) {
    returnUsers.push_back(session);
  }
  return returnUsers;
}

size_t RoomBase::GetActiveUser() {
  std::shared_lock lg{m_userLock};
  return m_Sessions.size();
}

void RoomBase::IntenalUpdateGameObjet() {
  for (auto& gameObj : m_gameObjects) {
    gameObj->Update();
  }
}

void RoomBase::IntenalUpdateProjectileObject() {
  for (auto projectileIter = m_projectileObjects.begin(); projectileIter != m_projectileObjects.end();) {
    (*projectileIter)->Update();
    if ((*projectileIter)->IsDestroy()) {  // check Expire
      projectileIter = m_projectileObjects.erase(projectileIter);
      continue;
    }
    projectileIter++;
  }
}

}  // namespace DreamWorld