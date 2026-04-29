#pragma once
#include <memory>
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThreadPool.h"

namespace DreamWorld {
class RoomBase;
class RoomThreadPool
    : public sh::Utility::ThreadPool,
      public sh::Utility::SingletonBase<RoomThreadPool> {
 public:
  void Init(const uint8_t threadNo);

  void Init(HANDLE handle);

  void InsertRoomUpdateEvent(std::shared_ptr<RoomBase> roomPtr);

 private:
  HANDLE m_otherHandle;  // 같은 thread pool 테스트
  bool m_isOtherHandle = false;
};
}  // namespace DreamWorld
