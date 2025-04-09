#pragma once
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <cstdint>
#include <memory>
#include <Utility/SingletonBase/Singleton.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Engine/CommonDefine.h>
#include <tbb/concurrent_queue.h>
#include "Session.h"

namespace Stress {
class SessionManager final
    : public sh::Utility::SingletonBase<SessionManager> {
 public:
  SessionManager() = default;

  void Init();

  std::shared_ptr<Stress::Session> OnConnect(SOCKET sock, sh::IO_Engine::IO_TYPE, HANDLE ioHandle, sh::IO_Engine::RecvHandler recvHandler);

  void OnDisconnect(std::shared_ptr<Stress::Session>& sessionPtr);

 private:
  uint32_t GetUniqueNo();

 private:
  sh::Utility::ObjectPool<Stress::Session> m_sessionPool;
  std::mutex m_sessionLock;
  std::unordered_map<uint32_t, sh::IO_Engine::ISessionPtr> m_activeSessions;  // k: uniqueNo, v: session ptr
  std::atomic<uint32_t> m_uniqueNo{0};                                        // seq uniqueNo
  tbb::concurrent_queue<uint32_t> m_restUniqueNo;                             // rest uniqueNo
};
}  // namespace Stress
