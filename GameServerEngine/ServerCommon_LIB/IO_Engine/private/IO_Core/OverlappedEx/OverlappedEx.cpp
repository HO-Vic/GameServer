#include "pch.h"
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <IO_Core/OverlappedEvent/IOverlappedEvent.h>

namespace sh::IO_Engine {
void OverlappedEx::operator()(const size_t ioByte) {
  m_overlappedEvent->Execute(this, m_type, ioByte);
}
}  // namespace sh::IO_Engine