#pragma once
#include "./ISendContextImpl.h"

namespace sh::IO_Engine {
class UDP_SendContextImpl final
    : public ISendContextImpl {
 public:
  UDP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock) {
  }

  virtual void DoSend(OverlappedPtr& session, const BYTE* data, const size_t len) override;

  virtual void SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) override;

 protected:
  virtual void SendExecute(OverlappedEx* overlappedEx) override;
};
}  // namespace sh::IO_Engine
