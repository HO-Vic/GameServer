#include "stdafx.h"
#include "RoomBase.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Character/ChracterObject.h"
#include "../GameObject/Projectile/ProjectileObject.h"

namespace DreamWorld {
RoomBase::RoomBase(const uint16_t maxExecuteJobCnt)
    : sh::Utility::LFJobQ_MT("RoomBase"), m_maxExecJobCnt(maxExecuteJobCnt) {
}

void RoomBase::Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte) {
  Update();
}

bool RoomBase::InsertPlayer(Session* caster) {
  std::lock_guard<std::shared_mutex> lg{m_userLock};
  //
  return false;
}

void RoomBase::DiscardPlayer(Session* caster) {
  std::lock_guard<std::shared_mutex> lg{m_userLock};
  //
}

void RoomBase::Broadcast(PacketHeader*, Session* caster) {
  std::shared_lock<std::shared_mutex> lg{m_userLock};
}

void RoomBase::Broadcast(PacketHeader*, const std::vector<std::shared_ptr<Session>>& ignorePlayers) {
  std::shared_lock<std::shared_mutex> lg{m_userLock};
}

void RoomBase::Update() {
  DoJobs(0);  // 몇이 좋을지는 모르겠음
  IntenalUpdateGameObjet();
  IntenalUpdateProjectileObject();
}

void RoomBase::InsertProjectileObject(std::shared_ptr<ProjectileObject>&& projectileObject) {
  m_projectileObjects.push_back(projectileObject);
}

void RoomBase::InsertGameObject(std::shared_ptr<GameObject>&& gameObject) {
  m_gameObjects.push_back(gameObject);
}

void RoomBase::IntenalUpdateGameObjet() {
  for (auto& gameObj : m_gameObjects) {
    // gameObj->
  }
}

void RoomBase::IntenalUpdateProjectileObject() {
  for (auto projectileIter = m_projectileObjects.begin(); projectileIter != m_projectileObjects.end();) {
    if (true /*projectileIter->()*/) {
      projectileIter = m_projectileObjects.erase(projectileIter);
      continue;
    }
    projectileIter++;
  }
}

}  // namespace DreamWorld