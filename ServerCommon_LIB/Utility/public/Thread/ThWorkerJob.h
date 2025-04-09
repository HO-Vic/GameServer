#pragma once
#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <vector>
namespace sh::Utility {
// Thread Pool에서 일할 객체

enum WORKER_TYPE : uint8_t {
  NONE = 0,
  SEND = 1,
  RECV = 2,
  ACCEPT = 3,
  DISCONN = 4,
  WORK = 5,
  CONNECT = 6,
  TERMINATE = 7,
};
class IWorkerItem;
using WokerPtr = std::shared_ptr<IWorkerItem>;
class ThWorkerJob : public OVERLAPPED {
  struct DebugCaller {
    std::string caller;
    int32_t line;
    bool isRel = false;
  };

 public:
  ThWorkerJob();

  ~ThWorkerJob();

  ThWorkerJob(const WokerPtr& workItem, WORKER_TYPE type);

  ThWorkerJob(WokerPtr&& workItem, WORKER_TYPE type);

  void operator()(const DWORD ioByte, const uint64_t errorCode);

  WORKER_TYPE GetType() const {
    return m_type;
  }

  void SetType(WORKER_TYPE type) {
    m_type = type;
    ZeroMemory(this, sizeof(OVERLAPPED));
  }

  WokerPtr GetWorkerItem() const {
    return m_workItem;
  }

  void AddRelCallStack(const char* f, int32_t l);

 private:
  std::shared_ptr<IWorkerItem> m_workItem;
  WORKER_TYPE m_type;
};
}  // namespace sh::Utility
