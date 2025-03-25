#pragma once
#include <Windows.h>
#include <memory>
namespace sh::Utility {
// Thread Pool에서 일할 객체

enum WORKER_TYPE : uint8_t {
  NONE = 0,
  SEND = 1,
  RECV = 2,
  ACCEPT = 3,
  DISCONN = 4,
  WORK = 5,
  TERMINATE = 6,
};
class IWorkerItem;
using WokerPtr = std::shared_ptr<IWorkerItem>;
class ThWorkerJob : public OVERLAPPED {
 public:
  ThWorkerJob();

  ThWorkerJob(WokerPtr& workItem, WORKER_TYPE type);

  ThWorkerJob(WokerPtr&& workItem, WORKER_TYPE type);

  void operator()(const DWORD ioByte);

  WORKER_TYPE GetType() const {
    return m_type;
  }

  void SetType(WORKER_TYPE type) {
    m_type = type;
  }

  void Reset();

  WokerPtr GetWorkerItem() const {
    return m_workItem;
  }

 private:
  std::shared_ptr<IWorkerItem> m_workItem;
  WORKER_TYPE m_type;
};
}  // namespace sh::Utility
