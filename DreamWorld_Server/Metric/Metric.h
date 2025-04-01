#pragma once
#include <Windows.h>
#include <atomic>
#include <Utility/SingletonBase/Singleton.h>

namespace DreamWorld {
struct Metric {
  std::atomic_uint32_t roomExec = 0;

  std::atomic_uint32_t timerExec = 0;

  std::atomic_uint32_t timerAlreadyExec = 0;

  std::atomic_uint32_t timerImmediate = 0;

  std::atomic_uint32_t DBExec = 0;

  std::atomic_uint32_t totalReq = 0;

  void Reset() {
    roomExec = 0;
    timerExec = 0;
    DBExec = 0;
    totalReq = 0;
  }
};

class MetricSlot final
    : public sh::Utility::SingletonBase<MetricSlot> {
 public:
  void Init(bool isUse = false);

  const bool IsUse() const {
    return m_isUse;
  }

  Metric& SwapAndLoad();

  void RecordRoom();

  void RecordDB();

  void RecordTimerExec();

  void RecordTimerAlready();

  void RecordTimerImmediate();

 private:
  Metric m_metics[2];
  std::atomic_uint8_t m_index = 0;
  bool m_isUse = false;
};
}  // namespace DreamWorld
