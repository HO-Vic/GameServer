#include "pch.h"
#include "SessionManager.h"

namespace sh::EchoServer {
sh::EchoServer::SessionManager::SessionManager()
    : m_userSeqId(0) {
}

void SessionManager::Init(const uint32_t initSize) {
  m_sessionPool.InitSize(initSize);
}

void SessionManager::OnAccept(SOCKET sock, IO_Engine::IO_TYPE ioType, IO_Engine::RecvHandler recvHandle, HANDLE iocpHandle) {
  auto sessionPtr = m_sessionPool.MakeShared(sock, ioType, recvHandle, iocpHandle, m_userSeqId++);
  std::lock_guard lg{m_activeSessionLock};
  m_activeSessions.emplace(sessionPtr->GetUniqueNo(), sessionPtr);
}

void SessionManager::OnDisconnect(uint32_t uniqueNo) {
  std::lock_guard lg{m_activeSessionLock};
  m_activeSessions.erase(uniqueNo);
}
}  // namespace sh::EchoServer
