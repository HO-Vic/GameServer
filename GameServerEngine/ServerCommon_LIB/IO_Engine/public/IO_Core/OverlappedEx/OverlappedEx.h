#pragma once
#include <pch.h>

namespace sh::IO_Engine {
class IOverlappedEvent;
struct OverlappedEx {
  OVERLAPPED overlapped;
  std::shared_ptr<IOverlappedEvent> m_overlappedEvent;
  OVERLAPPED_EVENT_TYPE m_type;

  void operator()(const size_t ioByte);
};
}  // namespace sh::IO_Engine
