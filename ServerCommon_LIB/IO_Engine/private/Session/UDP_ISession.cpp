#include "pch.h"
#include <Session/UDP_ISession.h>
#include <Buffer/SendBuffer.h>
#include <Buffer/SendBufferPool.h>
#include <Session/UDP_IAgent.h>

namespace sh::IO_Engine {
uint32_t UDP_ISession::DoSend(std::shared_ptr<UDP_IAgent>& agentPtr, const BYTE* data, uint32_t len) {
  if (!m_isAlive) {
    return 0;
  }
  auto selfPtr = shared_from_this();
  auto sendBufferPtr = UDP_SingleSendBufferPool::GetInstance().MakeShared(agentPtr, selfPtr, data, len);
  return sendBufferPtr->DoSend(agentPtr->GetSocket(), m_toAddr);
}

#pragma region Deprecated
void UDP_IBatchSession::DoSend(SOCKET sock, const BYTE* data, uint32_t len) {
  auto bufferPtr = SendBufferPool::GetInstance().MakeShared(data, len);
  std::lock_guard<std::mutex> lg{m_bufferLock};
  if (!m_buffers.back().CanInsert(len)) {
    m_buffers.push_back(InternalBatchSendBuffer{});
  }
  m_buffers.back().m_buffers.push_back(bufferPtr);
  m_buffers.back().m_totalLen += len;
  // Sendable 확인 후 시도 및...
}

bool UDP_IBatchSession::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  return false;
}
#pragma endregion  // Deprecated
}  // namespace sh::IO_Engine
