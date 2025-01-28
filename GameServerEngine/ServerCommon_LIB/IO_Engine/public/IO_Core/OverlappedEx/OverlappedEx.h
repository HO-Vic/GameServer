#pragma once
#include <pch.h>

namespace sh::IO_Engine {
class IOverlappedEvent;
class OverlappedEx {
 public:
  OverlappedEx(std::shared_ptr<IOverlappedEvent>& overlappedEvent, const OVERLAPPED_EVENT_TYPE type)
      : m_overlappedEvent(overlappedEvent), m_type(type) {
    ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
  }
  WSAOVERLAPPED overlapped;
  std::shared_ptr<IOverlappedEvent> m_overlappedEvent;
  OVERLAPPED_EVENT_TYPE m_type;

  void operator()(const size_t ioByte);
};
}  // namespace sh::IO_Engine
