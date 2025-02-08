#pragma once
#include <IO_Engine/IO_Core/IO_Core.h>

namespace sh::EchoServer {
class EchoServer {
 public:
  EchoServer();

  EchoServer(const uint16_t ioThreadNo);

  void Start();

 private:
  IO_Engine::IO_Core m_ioCore;
};
}  // namespace sh::EchoServer
