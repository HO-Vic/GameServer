#include "pch.h"
#include <Session/UDP_ISession.h>
#include <Buffer/SendBuffer.h>
#include <Buffer/SendBufferPool.h>

namespace sh::IO_Engine {
void UDP_ISession::DoSend(SOCKET sock, const BYTE* data, uint32_t len) const {
  auto sendBufferPtr = UDP_SingleSendBufferPool::GetInstance().MakeShared(data, len);
  sendBufferPtr->DoSend(sock, m_toAddr);
}
}  // namespace sh::IO_Engine
