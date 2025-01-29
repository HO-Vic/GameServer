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

int32_t SendContext::DoSend(OverlappedPtr& session, const BYTE* sendPacket, const size_t len) {
  auto errorNo = m_sendContextImpl->DoSend(session, sendPacket, len);
  if (WSA_IO_PENDING == errorNo) {
    errorNo = 0;
  }
  return errorNo;
}

int32_t SendContext::SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) {
  auto errorNo = m_sendContextImpl->SendComplete(overlappedEx, ioByte);
  if (WSA_IO_PENDING == errorNo) {
    errorNo = 0;
  }
  return errorNo;
}
}  // namespace sh::IO_Engine
