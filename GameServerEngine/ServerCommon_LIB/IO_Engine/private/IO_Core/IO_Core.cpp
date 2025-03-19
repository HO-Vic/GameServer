#include <pch.h>
#include <IO_Core/IO_Core.h>
#include <IO_Core/OverlappedEx/OverlappedEx.h>

namespace sh::IO_Engine {
IO_Core::IO_Core(const uint8_t ioThreadNo)
    : m_threadPool(ioThreadNo) {
  WSADATA wsaData{};
  if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) {
    assert("WSAData init fail");
  }
}

void IO_Core::Init() {
  m_threadPool.Init();
}

void IO_Core::Start() {
  m_threadPool.Start();
}

HANDLE IO_Core::GetHandle() const {
  return m_threadPool.GetHandle();
}

}  // namespace sh::IO_Engine
