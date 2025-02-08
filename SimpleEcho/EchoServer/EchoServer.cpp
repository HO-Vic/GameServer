#include <pch.h>
#include "EchoServer.h"

namespace sh::EchoServer {
EchoServer::EchoServer()
    : m_ioCore(4) {
  m_ioCore.Init();
}

EchoServer::EchoServer(const uint16_t ioThreadNo)
    : m_ioCore(ioThreadNo) {
}

void EchoServer::Start() {
  m_ioCore.Start();
}

}  // namespace sh::EchoServer
