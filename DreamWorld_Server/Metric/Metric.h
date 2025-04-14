#pragma once
#include <Windows.h>
#include <atomic>
#include <Utility/SingletonBase/Singleton.h>

namespace DreamWorld {
struct Metric {
  std::atomic_uint64_t roomExec = 0;

  std::atomic_uint64_t timerExec = 0;

  std::atomic_uint64_t timerAlreadyExec = 0;

  std::atomic_uint64_t timerImmediate = 0;

  std::atomic_uint64_t DBExec = 0;

  std::atomic_uint64_t jobTotalCnt = 0;
  std::atomic_uint64_t jobUsingCnt = 0;
  std::atomic_uint64_t jobAddCnt = 0;

  std::atomic_uint64_t timerJobTotalCnt = 0;
  std::atomic_uint64_t timerJobUsingCnt = 0;
  std::atomic_uint64_t timerJobAddCnt = 0;

  std::atomic_uint64_t roomTotalCnt = 0;
  std::atomic_uint64_t roomUsingCnt = 0;
  std::atomic_uint64_t roomAddCnt = 0;

  std::atomic_uint64_t totalReq = 0;

  void Reset() {
    roomExec = 0;
    timerExec = 0;
    timerAlreadyExec = 0;
    timerImmediate = 0;
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
