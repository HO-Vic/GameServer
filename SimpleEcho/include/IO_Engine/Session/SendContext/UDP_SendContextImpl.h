#pragma once
#include "./ISendContextImpl.h"

namespace sh::IO_Engine {
class UDP_SendContextImpl final
    : public ISendContextImpl {
 public:
  UDP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock) {
  }

  virtual int32_t DoSend(OverlappedPtr& session, const BYTE* data, const size_t len) override;

  virtual int32_t SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) override;

 protected:
  virtual int32_t SendExecute(OverlappedEx* overlappedEx) override;
};
}  // namespace sh::IO_Engine
