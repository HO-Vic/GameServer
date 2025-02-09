#pragma once
#include <IO_Engine/Session/ISession.h>

namespace sh::EchoServer {
class Session final
    : public IO_Engine::ISession {
 public:
  virtual void Disconnect() override;

 private:
};
}  // namespace sh::EchoServer
