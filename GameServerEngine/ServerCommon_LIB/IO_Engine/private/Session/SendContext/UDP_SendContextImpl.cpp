#include <pch.h>
#include <Session/SendContext/UDP_SendContextImpl.h>

namespace sh::IO_Engine {
int32_t UDP_SendContextImpl::DoSend(OverlappedPtr& session, const BYTE* data, const size_t len) {
  return 0;
}

int32_t UDP_SendContextImpl::SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) {
  return 0;
}

int32_t UDP_SendContextImpl::SendExecute(OverlappedEx* overlappedEx) {
  return 0;
}
}  // namespace sh::IO_Engine
