#include "stdafx.h"
#include "SessionMananger.h"
#include <unordered_map>

namespace DreamWorld {
void SessionMananger::Init() {
  m_userPool.InitSize(100);
  m_uniqueNo = 1;
}
void SessionMananger::OnAccept(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandle, HANDLE iocpHandle) {
  auto userRef = m_userPool.MakeShared(sock, ioType, std::move(recvHandle), iocpHandle, m_uniqueNo++);

  std::lock_guard<std::mutex> lg(m_lock);
  m_activeUsers.try_emplace(userRef->GetUniqueNo(), std::move(userRef));
}

void SessionMananger::OnDisconnect(uint32_t userNo) {
  std::lock_guard<std::mutex> lg(m_lock);
  m_activeUsers.erase(userNo);
}
void SessionMananger::BroadCast(std::vector<uint32_t> ignoreSessionId) {
  std::lock_guard<std::mutex> lg(m_lock);
  //
}
}  // namespace DreamWorld
