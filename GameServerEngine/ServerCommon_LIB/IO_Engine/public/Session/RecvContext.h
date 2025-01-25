#pragma once
#include <pch.h>

namespace sh {
namespace IO_Engine {
class RecvContext {
  constexpr static uint32_t MAX_RECV_BUF_SIZE = 1024;

 public:
  RecvContext(SOCKET sock)
      : m_socket(sock) {
  }

  const uint32_t GetStorableLen() const;

  char* GetStoreStartBuffer();

 private:
  SOCKET m_socket;
  uint32_t m_remainLen;
  char m_buffer[MAX_RECV_BUF_SIZE];
};
}
}  // namespace sh::IO_Engine
