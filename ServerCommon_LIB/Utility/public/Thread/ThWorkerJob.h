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
  DISCONN = 4,        // IO 작업
  WORK = 5,           // DB, 게임 틱 업데이트 용
  CONNECT = 6,        // 클라이언트 비동기 연결
  TERMINATE = 7,      // 쓰레드 풀 종료
  FORCE_DISCONN = 8,  // 동접 모듈용 클라이언트 강제 연결 해제
  DESTROY = 9,        // UDP_Session 파괴용
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

 private:
  std::shared_ptr<IWorkerItem> m_workItem;
  WORKER_TYPE m_type;
};
}  // namespace sh::Utility
