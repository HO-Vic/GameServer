#pragma once
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThreadPool.h"

namespace DreamWorld {
class DBThreadPool
    : public sh::Utility::ThreadPool,
      public sh::Utility::SingletonBase<DBThreadPool> {
 public:
  // void InsertDBJob();

  void Init(const uint8_t threadNo);
};
}  // namespace DreamWorld
