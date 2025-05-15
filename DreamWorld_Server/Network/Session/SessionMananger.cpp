#include "stdafx.h"
#include "SessionMananger.h"
#include <spdlog/common.h>
#include <unordered_map>
#include <../LogManager/LogManager.h>

namespace DreamWorld {
void SessionMananger::Init() {
  m_userPool.InitSize(4500);
  m_uniqueNo = 1;
  m_currentActiveUser = 0;
}

void SessionMananger::OnAccept(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::TCP_RecvHandler recvHandle, HANDLE iocpHandle) {
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
  m_currentActiveUser++;
}

void SessionMananger::OnDisconnect(uint32_t userNo) {
  {
    std::lock_guard<std::mutex> lg(m_lock);
    m_activeUsers.erase(userNo);
  }
  m_currentActiveUser--;
}

void SessionMananger::BroadCast(std::vector<uint32_t> ignoreSessionId) {
  std::lock_guard<std::mutex> lg(m_lock);
  //
}

const uint32_t SessionMananger::GetCurrentActiveUserCnt() const {
  return m_currentActiveUser;
}
}  // namespace DreamWorld
