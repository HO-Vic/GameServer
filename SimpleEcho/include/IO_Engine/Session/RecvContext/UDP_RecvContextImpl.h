#pragma once
#include <cstdint>
#include <Session/RecvContext/IRecvContextImpl.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>

namespace sh::IO_Engine {
class UDP_RecvContextImpl
    : public IRecvContextImpl {
 public:
  UDP_RecvContextImpl(SOCKET sock, RecvHandler&& recvHandler)
      : IRecvContextImpl(sock, std::move(recvHandler)) {
  }
  virtual int32_t RecvComplete(OverlappedEx* overlappedEx, size_t ioSize) override;

  virtual int32_t DoRecv(OverlappedEx* overlappedEx) override;
};
}  // namespace sh::IO_Engine
