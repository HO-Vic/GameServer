#pragma once
#include <cstdint>
#include <IO_Engine/IO_Core/IO_Core.h>

namespace sh::UDP_Echo {
class UDP_Server final {
 public:
  UDP_Server() = default;

  void Start();

  void Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize = 1500, const uint32_t sendBufferPoolSize = 1500);

 private:
  IO_Engine::IO_Core m_ioCore;
};
}  // namespace sh::UDP_Echo
