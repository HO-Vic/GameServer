#pragma once
#include "./ISendContextImpl.h"

namespace sh::IO_Engine {
class UDP_SendContextImpl final
    : public ISendContextImpl {
 public:
  UDP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock) {
  }

  virtual void DoSend(const BYTE* data, const size_t len) override;

  virtual void SendComplete(const size_t ioByte) override;

 private:
  virtual void SendExecute() override;
};
}  // namespace sh::IO_Engine
