#pragma once
#include <cstdint>
#include <IO_Core/OverlappedEx/OverlappedEx.h>
#include <Session/RecvContext/IRecvContextImpl.h>

namespace sh::IO_Engine {
class OverLappedEx;
class TCP_RecvContextImpl
    : public IRecvContextImpl {
 public:
  TCP_RecvContextImpl(SOCKET sock, RecvHandler&& recvHandler)
      : IRecvContextImpl(sock, std::move(recvHandler)) {
  }

  virtual int32_t RecvComplete(OverlappedEx* overlappedEx, size_t ioSize) override;

  virtual int32_t DoRecv(OverlappedEx* overlappedEx) override;
};
}  // namespace sh::IO_Engine
