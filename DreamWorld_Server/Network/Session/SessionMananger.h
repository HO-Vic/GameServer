#pragma once
#include <mutex>
#include "Session.h"
#include "Utility/Pool/ObjectPool.h"
#include "Utility/SingletonBase/Singleton.h"

namespace DreamWorld {
class SessionMananger
    : public sh::Utility::SingletonBase<SessionMananger> {
 public:
  void Init();

  void OnAccept(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandle, HANDLE iocpHandle);

  void OnDisconnect(uint32_t userNo);

  void BroadCast(std::vector<uint32_t> ignoreSessionId);

 private:
  sh::Utility::ObjectPool<Session> m_userPool;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> m_activeUsers;
  std::mutex m_lock;
  std::atomic<uint32_t> m_uniqueNo;
};
}  // namespace DreamWorld
