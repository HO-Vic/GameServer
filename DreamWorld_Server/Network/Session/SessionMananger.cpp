#include "stdafx.h"
#include "SessionMananger.h"
#include <spdlog/common.h>
#include <unordered_map>
#include <../LogManager/LogManager.h>

namespace DreamWorld {
void SessionMananger::Init() {
  m_userPool.InitSize(100);
  m_uniqueNo = 1;
}
void SessionMananger::OnAccept(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandle, HANDLE iocpHandle) {
  auto userRef = m_userPool.MakeShared(sock, ioType, std::move(recvHandle), iocpHandle, m_uniqueNo++);
  if (NULL == CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), iocpHandle, reinterpret_cast<uint64_t>(userRef.get()), 0)) {
    WRITE_LOG(logLevel::err, "{}({}) > Can not Regist sock To ioHandle!!", __FUNCTION__, __LINE__);
    return;
  }

  {
    std::lock_guard<std::mutex> lg(m_lock);
    m_activeUsers.try_emplace(userRef->GetUniqueNo(), userRef);
  }
  userRef->StartRecv();
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
