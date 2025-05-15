#pragma once
#include <memory>
#include <Windows.h>
#include <cstdint>
#include <unordered_map>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>

namespace sh::Utility {
class ThWorkerJob;
}

namespace Stress {
class Session;
using SessionPtr = std::shared_ptr<Session>;
class SessionBatchUpdater
    : public sh::Utility::DoubleJobQ_MT,
      public sh::Utility::IWorkerItem {
  friend class SessionBatchUpdaters;

 public:
  void InsertSession(SessionPtr session);

  void DiscardSession(SessionPtr session);

  virtual bool Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

 private:
  void IntenalUpdate();

  void ReserveNextUpdate();

 private:
  std::unordered_map<uint32_t, SessionPtr> m_sessions;  // jobQ로 싱글 쓰레드로 동작하게
};
}  // namespace Stress
