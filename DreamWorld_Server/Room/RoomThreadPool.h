#pragma once
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThreadPool.h"

namespace DreamWorld {
class RoomThreadPool
    : public sh::Utility::ThreadPool,
      public sh::Utility::SingletonBase<RoomThreadPool> {
 public:
  void Init(const uint8_t threadNo);

  void InsertRoomUpdateEvent();

 private:
};
}  // namespace DreamWorld
