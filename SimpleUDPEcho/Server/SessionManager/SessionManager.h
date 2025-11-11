#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include <Winsock2.h>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <cstdint>
#include <IO_Engine/CommonDefine.h>

namespace sh::IO_Engine {
class UDP_ISession;
}

namespace sh::UDP_Echo {
class SessionManager
    : public Utility::SingletonBase<SessionManager> {
 public:
  void Init();

  IO_Engine::UDP_ISessionPtr InsertSession(uint64_t hashKey, const sockaddr_in& toAddr);

  void EraseSession(uint64_t uniqueNo);

  IO_Engine::UDP_ISessionPtr GetSession(uint64_t hashKey);

 private:
  std::mutex m_lock;

  std::unordered_map<uint64_t, std::shared_ptr<IO_Engine::UDP_ISession>> m_sessions;

  std::atomic_uint32_t m_seqNo = 0;
};
}  // namespace sh::UDP_Echo
