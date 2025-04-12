#include <pch.h>
#include <Session/SendContext/SendContext.h>
#include <Session/SendContext/TCP_SendContextImpl.h>
#include <Session/SendContext/UDP_SendContextImpl.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Thread/ThWorkerJob.h>

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

int32_t SendContext::DoSend(Utility::WorkerPtr& session, const BYTE* sendPacket, const size_t len) {
  return m_sendContextImpl->DoSend(session, sendPacket, len);
}

int32_t SendContext::SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) {
  return m_sendContextImpl->SendComplete(thWorkerJob, ioByte);
}
}  // namespace sh::IO_Engine
