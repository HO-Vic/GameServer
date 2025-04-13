#pragma once
#include <cstdint>
#include "./ISendContext.h"

namespace sh::IO_Engine {
class UDP_SendContext final
    : public ISendContext {
 public:
  UDP_SendContext(SOCKET sock)
      : ISendContext(sock) {
  }

  virtual int32_t DoSend(Utility::WorkerPtr session, const BYTE* data, const size_t len) override;

  virtual int32_t SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) override;

 private:
  virtual int32_t SendExecute(Utility::ThWorkerJob* thWorkerJob) override;
};
}  // namespace sh::IO_Engine
