#pragma once
#include <cstdint>
#include <Session/RecvContext/IRecvContext.h>

namespace sh::IO_Engine {
class UDP_RecvContext
    : public IRecvContext {
 public:
  UDP_RecvContext(SOCKET sock, RecvHandler&& recvHandler)
      : IRecvContext(sock, std::move(recvHandler)) {
  }
  virtual int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize) override;

  virtual int32_t StartRecv(Utility::WorkerPtr& session) override;

 protected:
  virtual int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob) override;
};
}  // namespace sh::IO_Engine
