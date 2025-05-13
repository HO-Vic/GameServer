#pragma once
#include <cstdint>

namespace sh {
namespace Utility {
class ThWorkerJob;
class IWorkerItem;
using WorkerPtr = std::shared_ptr<IWorkerItem>;
}
}  // namespace sh::Utility

namespace sh::IO_Engine {
class UDP_SendContext final {
 public:
  UDP_SendContext() {
  }

  int32_t DoSend(Utility::WorkerPtr session, const BYTE* data, const size_t len);

  int32_t SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte);

 private:
  int32_t SendExecute(Utility::ThWorkerJob* thWorkerJob);
};
}  // namespace sh::IO_Engine
