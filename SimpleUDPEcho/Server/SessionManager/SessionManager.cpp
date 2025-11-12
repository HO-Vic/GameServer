#include "pch.h"
#include "SessionManager.h"
#include "../Session/UDP_Session/UDP_Session.h"

namespace sh::UDP_Echo {
void SessionManager::Init() {
}

IO_Engine::UDP_SessionBasePtr SessionManager::InsertSession(uint64_t hashKey, const sockaddr_in& toAddr) {
  std::lock_guard<std::mutex> lg{m_lock};
  if (m_sessions.count(hashKey)) {
    return nullptr;
  }

  auto sessionPtr = std::make_shared<UDP_Session>(toAddr);
  m_sessions.emplace(hashKey, sessionPtr);
  return sessionPtr;
}

void SessionManager::EraseSession(uint64_t uniqueNo) {
  std::lock_guard<std::mutex> lg{m_lock};
  m_sessions.erase(uniqueNo);
}

IO_Engine::UDP_SessionBasePtr SessionManager::GetSession(uint64_t hashKey) {
  std::lock_guard<std::mutex> lg{m_lock};
  auto fIter = m_sessions.find(hashKey);
  if (fIter == m_sessions.end()) {
    return nullptr;
  }
  return fIter->second;
}

}  // namespace sh::UDP_Echo