#pragma once
#include <IO_Engine/Session/TCP_SessionBase.h>

namespace SimpleTCP {
class Session
    : public sh::IO_Engine::TCP_SessionBase {
 public:
  Session() = default;

  Session(SOCKET sock, sh::IO_Engine::TCP_RecvHandler recvHandler, HANDLE iocpHandle);

  ~Session() override;

  void OnDisconnect() override;

  // private:
};
}  // namespace SimpleTCP
