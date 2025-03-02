#pragma once
#include <memory>
#include "../../CommonDefine.h"

namespace sh::IO_Engine {
class OverlappedEx;
class IOverlappedEvent : public std::enable_shared_from_this<IOverlappedEvent> {
 public:
  virtual void Execute(OverlappedEx* overlappedEx, const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) = 0;
};
}  // namespace sh::IO_Engine
