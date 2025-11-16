#include "pch.h"
#include "IO_Core/SocketResetEvent/SocketResetEventPool.h"

namespace sh::IO_Engine {
void SocketResetEventPool::Init(uint32_t size, bool useMetric) {
  InitSize(size, useMetric);
}
}  // namespace sh::IO_Engine
