#pragma once
#include <cstdint>
#include <IO_Engine/CommonDefine.h>
#include <IO_Engine/IO_Core/IO_Core.h>

namespace sh::IO_Engine {
class UDP_IAgent;
}
namespace sh::UDP_Echo {
class UDP_Server final {
 public:
  UDP_Server() = default;

  void Start();

  bool Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize = 1500, const uint32_t sendBufferPoolSize = 1500);

  // agentPtr은 recv받는 주체?
  void RecvHandler(IO_Engine::UDP_IAgentPtr agentPtr, size_t ioByte, BYTE* bufferPtr, const sockaddr_in& addrInfo);

 private:
  IO_Engine::IO_Core m_ioCore;
  IO_Engine::UDP_IAgentPtr m_agent = nullptr;
};
}  // namespace sh::UDP_Echo
