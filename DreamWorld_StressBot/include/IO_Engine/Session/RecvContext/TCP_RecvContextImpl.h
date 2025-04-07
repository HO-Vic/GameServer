#pragma once
#include <cstdint>
#include <Session/RecvContext/IRecvContextImpl.h>

namespace sh::IO_Engine {
class TCP_RecvContextImpl
    : public IRecvContextImpl {
 public:
  TCP_RecvContextImpl(SOCKET sock, RecvHandler&& recvHandler)
      : IRecvContextImpl(sock, std::move(recvHandler)) {
  }

  virtual int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize) override;

  virtual int32_t DoRecv(Utility::ThWorkerJob* thWorkerJob) override;
};
}  // namespace sh::IO_Engine
