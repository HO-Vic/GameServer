#pragma once
#include <pch.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
class TerminateEvent
    : public IOverlappedEvent {
 public:
  virtual void Execute(const OVERLAPPED_EVENT_TYPE type, const size_t ioByte) override;
};

}  // namespace sh::IO_Engine