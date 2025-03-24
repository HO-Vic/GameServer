#pragma once
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThreadPool.h"
#include <memory>
#include <cstdint>

namespace DreamWorld {
class DBJobBase;
class DBThreadPool
    : public sh::Utility::ThreadPool,
      public sh::Utility::SingletonBase<DBThreadPool> {
 public:
  void InsertDBJob(std::shared_ptr<DBJobBase>&& jobPtr);

  void Init(const uint8_t threadNo);
};
}  // namespace DreamWorld
