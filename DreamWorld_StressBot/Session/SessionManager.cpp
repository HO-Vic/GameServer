#include "pch.h"
#include "SessionManager.h"
#include <IO_Engine/CommonDefine.h>
#include <Utility/Job/Job.h>
#include <Utility/Pool/ObjectPool.h>
#include "../GlobalObjectPool/GlobalObjectPool.h"
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
  {
    std::lock_guard<std::mutex> lg{m_sessionLock};
    m_activeSessions.insert({uniqueNo, sessionPtr});
  }
  IntenalInsertRenderSession(sessionPtr);
  return sessionPtr;
}

void Stress::SessionManager::OnDisconnect(std::shared_ptr<Stress::Session>& sessionPtr) {
  IntenalDiscardRenderSession(sessionPtr);
  //{
  //  std::lock_guard<std::mutex> lg{m_sessionLock};
  //  m_activeSessions.erase(sessionPtr->GetUniqueNo());
  //}
}

void SessionManager::InsertRestUniqueNo(const uint32_t uniqueNo) {
  m_restUniqueNo.push(uniqueNo);
}

std::shared_ptr<Session> SessionManager::GetForceDiscardSession() {
  std::lock_guard<std::mutex> lg{m_sessionLock};
  if (m_activeSessions.empty()) {
    return nullptr;
  }
  return m_activeSessions.begin()->second;
}

const std::unordered_map<uint32_t, std::shared_ptr<Session>>& SessionManager::GetSessionForRender() {
  m_renderJobQ.DoJobs();
  return m_dupSessionsForRender;
}

void SessionManager::IntenalInsertRenderSession(std::shared_ptr<Session>& session) {
  m_renderJobQ.InsertJob(
      GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(
          [session]() mutable {
            SessionManager::GetInstance().m_dupSessionsForRender.insert({session->GetUniqueNo(), std::move(session)});
          }));
}

void SessionManager::IntenalDiscardRenderSession(std::shared_ptr<Session>& session) {
  auto discardNo = session->GetUniqueNo();
  m_renderJobQ.InsertJob(
      GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique(
          [discardNo]() {
            SessionManager::GetInstance().m_dupSessionsForRender.erase(discardNo);
          }));
}

uint32_t SessionManager::GetUniqueNo() {
  uint32_t uniqueNo = 0;
  if (m_restUniqueNo.try_pop(uniqueNo)) {
    return uniqueNo;
  }
  return m_uniqueNo++;
}
}  // namespace Stress
