#include "pch.h"
#include <Session/RecvContext/UDP_RecvContext.h>

namespace sh::IO_Engine {
int32_t UDP_RecvContext::RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize) {
  return 0;
}

int32_t UDP_RecvContext::DoRecv(Utility::ThWorkerJob* thWorkerJob) {
  return 0;
}
}  // namespace sh::IO_Engine
