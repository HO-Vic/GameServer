#include "pch.h"
#include "SessionManager.h"
#include "../LogManager/LogManager.h"

namespace sh::EchoServer {
sh::EchoServer::SessionManager::SessionManager()
    : m_userSeqId(0) {
}

void SessionManager::Init(const uint32_t initSize) {
  m_sessionPool.InitSize(initSize);
}

void SessionManager::OnAccept(SOCKET sock, IO_Engine::IO_TYPE ioType, IO_Engine::RecvHandler recvHandle, HANDLE iocpHandle) {
  auto sessionPtr = m_sessionPool.MakeShared(sock, ioType, recvHandle, iocpHandle, m_userSeqId++);
  CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), iocpHandle, reinterpret_cast<uint64_t>(sessionPtr.get()), 0);
  {
    std::lock_guard lg{m_activeSessionLock};
    m_activeSessions.emplace(sessionPtr->GetUniqueNo(), sessionPtr);
  }
  sessionPtr->StartRecv();
  WRITE_LOG(spdlog::level::info, "{}({}) > Start Recv", __FUNCTION__, __LINE__);
}

void SessionManager::OnDisconnect(uint32_t uniqueNo) {
  std::lock_guard lg{m_activeSessionLock};
  m_activeSessions.erase(uniqueNo);
}
}  // namespace sh::EchoServer
