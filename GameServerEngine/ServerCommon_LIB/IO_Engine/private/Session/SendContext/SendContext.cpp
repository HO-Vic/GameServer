#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/SendContext/TCP_SendContextImpl.h>
#include <Session/SendContext/UDP_SendContextImpl.h>

namespace sh::IO_Engine {
SendContext::SendContext(SOCKET sock, const IO_TYPE sendType)
    : m_sendContextImpl(nullptr) {
  if (IO_TYPE::UDP == sendType) {
    m_sendContextImpl = new UDP_SendContextImpl(sock);
    return;
  }
  m_sendContextImpl = new TCP_SendContextImpl(sock);
}

SendContext::~SendContext() {
  delete m_sendContextImpl;
}

void SendContext::DoSend(const BYTE* sendPacket, const size_t len) {
  m_sendContextImpl->DoSend(sendPacket, len);
}

void SendContext::SendComplete(const size_t ioByte) {
  m_sendContextImpl->SendComplete(ioByte);
}
}  // namespace sh::IO_Engine
