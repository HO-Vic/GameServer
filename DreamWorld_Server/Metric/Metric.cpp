#include "stdafx.h"
#include "Metric.h"
#include "../Room/RoomManager.h"
#include "../ObjectPools.h"
#include <Utility/Job/Job.h>
#include "../Timer/TimerJob.h"

namespace DreamWorld {
void MetricSlot::Init(bool isUse) {
  m_isUse = isUse;
}

Metric& MetricSlot::SwapAndLoad() {
  auto taskIndex = m_index.load();
  {
    m_metics[taskIndex].jobTotalCnt.store(ObjectPool<sh::Utility::Job>::GetInstance().GetTotalCnt());
    m_metics[taskIndex].jobAddCnt.store(ObjectPool<sh::Utility::Job>::GetInstance().GetAddedCnt());
    m_metics[taskIndex].jobUsingCnt.store(ObjectPool<sh::Utility::Job>::GetInstance().GetUsingCnt());

    m_metics[taskIndex].timerJobTotalCnt.store(ObjectPool<TimerJob>::GetInstance().GetTotalCnt());
    m_metics[taskIndex].timerJobAddCnt.store(ObjectPool<TimerJob>::GetInstance().GetAddedCnt());
    m_metics[taskIndex].timerJobUsingCnt.store(ObjectPool<TimerJob>::GetInstance().GetUsingCnt());

    m_metics[taskIndex].roomTotalCnt.store(RoomManager::GetInstance().GetRoomPool().GetTotalCnt());
    m_metics[taskIndex].roomAddCnt.store(RoomManager::GetInstance().GetRoomPool().GetAddedCnt());
    m_metics[taskIndex].roomUsingCnt.store(RoomManager::GetInstance().GetRoomPool().GetUsingCnt());
  }
  auto otherIndex = (taskIndex + 1) % 2;
  m_metics[otherIndex].Reset();
  m_index.store(otherIndex);

  return m_metics[taskIndex];
}

void MetricSlot::RecordRoom() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].roomExec.fetch_add(1);
}

void MetricSlot::RecordDB() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].DBExec.fetch_add(1);
}

void MetricSlot::RecordTimerExec() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].timerExec.fetch_add(1);
}

void MetricSlot::RecordTimerAlready() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].timerAlreadyExec.fetch_add(1);
}

void MetricSlot::RecordTimerImmediate() {
  if (!m_isUse) {
    return;
  }
  auto taskIndex = m_index.load();

  m_metics[taskIndex].totalReq.fetch_add(1);
  m_metics[taskIndex].timerImmediate.fetch_add(1);
}

}  // namespace DreamWorld