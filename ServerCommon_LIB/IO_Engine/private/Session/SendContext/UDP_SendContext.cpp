#include <pch.h>
#include <Session/SendContext/UDP_SendContext.h>

namespace sh::IO_Engine {
int32_t UDP_SendContext::DoSend(Utility::WorkerPtr& session, const BYTE* data, const size_t len) {
  return 0;
}

int32_t UDP_SendContext::SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) {
  return 0;
}

int32_t UDP_SendContext::SendExecute(Utility::ThWorkerJob* thWorkerJob) {
  return 0;
}
}  // namespace sh::IO_Engine
