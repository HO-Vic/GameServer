#include "pch.h"
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
OverlappedEx::OverlappedEx()
    : m_overlappedEvent(nullptr), m_type(OVERLAPPED_EVENT_TYPE::NONE) {
  ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
}
void OverlappedEx::operator()(const size_t ioByte) {
  m_overlappedEvent->Execute(this, m_type, ioByte);
}
}  // namespace sh::IO_Engine