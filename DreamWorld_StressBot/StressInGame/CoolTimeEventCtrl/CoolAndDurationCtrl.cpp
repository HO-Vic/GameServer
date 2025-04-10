#include "pch.h"
#include "CoolAndDurationCtrl.h"

namespace Stress {
const bool CoolAndDurationCtrl::IsAbleExecute() {
  auto nowTime = std::chrono::high_resolution_clock::now();
  auto lastExecuteDurationTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastExecTime);
  // 쿨 타임보다 (마지막 시간 - 지금)이 더 큰 값 수행 가능
  if (m_coolTime <= lastExecuteDurationTime) {
    m_lastExecTime = nowTime;
    return true;
  }
  return false;
}

void CoolAndDurationCtrl::ForceExecute() {
  m_lastExecTime = std::chrono::high_resolution_clock::now();
}

const CoolAndDurationCtrl::TIME& CoolAndDurationCtrl::GetLastExeTime() const {
  return m_lastExecTime;
}

void CoolAndDurationCtrl::ResetCoolTime() {
  m_lastExecTime -= m_coolTime;
}
}  // namespace Stress
