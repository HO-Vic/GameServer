#pragma once
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <WinSock2.h>
#include <memory>
#include <atomic>
#include <Utility/Pool/ObjectPool.h>
#include <Utility/SingletonBase/Singleton.h>
#include <IO_Engine/CommonDefine.h>
#include "Session.h"

namespace SimpleTCP {

class SessionManager
    : public sh::Utility::SingletonBase<SessionManager> {
 public:
  SessionManager();

  void Init(const uint32_t initSize);

  void OnAccept(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::TCP_RecvHandler recvHandle, HANDLE iocpHandle);

  void OnDisconnect(uint32_t uniqeNo);

 private:
  sh::Utility::ObjectPool<Session> m_sessionPool;
  std::mutex m_activeSessionLock;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> m_activeSessions;
  std::atomic<uint32_t> m_userSeqId;
};
}  // namespace SimpleTCP
