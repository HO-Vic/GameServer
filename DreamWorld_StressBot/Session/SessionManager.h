#pragma once
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <cstdint>
#include <memory>
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Engine/CommonDefine.h>
#include <Utility/Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>
#include <tbb/concurrent_queue.h>
#include "Session.h"

namespace Stress {
class SessionManager final
    : public sh::Utility::SingletonBase<SessionManager> {
 public:
  SessionManager() = default;

  void Init();

  std::shared_ptr<Stress::Session> OnConnect(SOCKET sock, sh::IO_Engine::IO_TYPE, HANDLE ioHandle, sh::IO_Engine::TCP_RecvHandler TCP_RecvHandler);

  void OnDisconnect(std::shared_ptr<Stress::Session>& sessionPtr);

  void InsertRestUniqueNo(const uint32_t uniqueNo);

  std::shared_ptr<Session> GetForceDiscardSession();

  const std::unordered_map<uint32_t, std::shared_ptr<Session>>& GetSessionForRender();

 private:
  void IntenalInsertRenderSession(std::shared_ptr<Session>& session);

  void IntenalDiscardRenderSession(std::shared_ptr<Session>& session);

 private:
  uint32_t GetUniqueNo();

 private:
  sh::Utility::ObjectPool<Stress::Session> m_sessionPool;
  std::mutex m_sessionLock;
  std::unordered_map<uint32_t, std::shared_ptr<Stress::Session>> m_activeSessions;  // k: uniqueNo, v: session ptr
  std::atomic<uint32_t> m_uniqueNo{0};             // seq uniqueNo
  tbb::concurrent_queue<uint32_t> m_restUniqueNo;  // rest uniqueNo

  // 렌더링할 때마다 lock 잡고 하는건 별로
  sh::Utility::DoubleJobQ_MT m_renderJobQ;                                                // 섹션 Insert, Discard를 위한
  std::unordered_map<uint32_t, std::shared_ptr<Stress::Session>> m_dupSessionsForRender;  // k: uniqueNo, v: session ptr
 public:
};
}  // namespace Stress
