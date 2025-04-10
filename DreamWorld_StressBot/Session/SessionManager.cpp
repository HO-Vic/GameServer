#include "pch.h"
#include "SessionManager.h"
#include <IO_Engine/CommonDefine.h>
#include "Session.h"
#include "../LogManager/LogManager.h"

namespace Stress {
void Stress::SessionManager::Init() {
  m_sessionPool.InitSize(4000);
}

std::shared_ptr<Stress::Session> SessionManager::OnConnect(SOCKET sock, sh::IO_Engine::IO_TYPE, HANDLE ioHandle, sh::IO_Engine::RecvHandler recvHandler) {
  auto uniqueNo = GetUniqueNo();
  auto sessionPtr = m_sessionPool.MakeShared(sock, sh::IO_Engine::IO_TYPE::TCP, recvHandler, ioHandle, uniqueNo);
  if (nullptr == sessionPtr) {
    WRITE_LOG(logLevel::err, "{}({}) > Session Pool MakeShared Fail!!", __FUNCTION__, __LINE__);
    return nullptr;
  }
  std::lock_guard<std::mutex> lg{m_sessionLock};
  m_activeSessions.insert({uniqueNo, sessionPtr});
  return sessionPtr;
}

void Stress::SessionManager::OnDisconnect(std::shared_ptr<Stress::Session>& sessionPtr) {
  std::lock_guard<std::mutex> lg{m_sessionLock};
  m_activeSessions.erase(sessionPtr->GetUniqueNo());
}

void SessionManager::InsertRestUniqueNo(const uint32_t uniqueNo) {
  m_restUniqueNo.push(uniqueNo);
}

uint32_t SessionManager::GetUniqueNo() {
  uint32_t uniqueNo = 0;
  if (m_restUniqueNo.try_pop(uniqueNo)) {
    return uniqueNo;
  }
  return m_uniqueNo++;
}
}  // namespace Stress
