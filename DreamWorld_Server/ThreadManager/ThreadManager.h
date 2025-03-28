#pragma once
#include "../PCH/stdafx.h"
#include <../SingletonBase.h>
#include <Utility/Thread/ThreadManager.h>

namespace DreamWorld {
class ThreadManager
    : public SingletonBase<ThreadManager>,
      public sh::Utility::ThreadManager {
  friend SingletonBase;

 public:
  ThreadManager() = default;
};
}  // namespace DreamWorld
