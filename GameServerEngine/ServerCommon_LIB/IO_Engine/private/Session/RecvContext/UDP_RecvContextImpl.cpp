#include "pch.h"
#include <Session/RecvContext/UDP_RecvContextImpl.h>

namespace sh::IO_Engine {
int32_t UDP_RecvContextImpl::RecvComplete(OverlappedEx* overlappedEx, size_t ioSize) {
  return 0;
}

int32_t UDP_RecvContextImpl::DoRecv(OverlappedEx* overlappedEx) {
  return 0;
}
}  // namespace sh::IO_Engine
