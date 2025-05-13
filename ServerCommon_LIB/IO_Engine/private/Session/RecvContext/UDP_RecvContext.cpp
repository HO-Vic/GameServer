#include "pch.h"
#include "pch.h"
#include <Session/RecvContext/UDP_RecvContext.h>

namespace sh::IO_Engine {
int32_t UDP_RecvContext::RecvComplete(Utility::ThWorkerJob* thWorkerJob, size_t ioSize, SOCKET sock) {
  auto sessionPtr = m_sessionPtr.lock();  // 세션 종료라면 더 이상 IO xx
  // WSARecvFrom();
  return 0;
}

int32_t UDP_RecvContext::DoRecv(Utility::ThWorkerJob* thWorkerJob, SOCKET sock) {
  return 0;
}
}  // namespace sh::IO_Engine
