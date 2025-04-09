#pragma once
#include <Windows.h>
#include <winsock.h>
#include <IO_Engine/Session/ISession.h>
#include <IO_Engine/CommonDefine.h>

namespace Stress {
class Session
    : public sh::IO_Engine::ISession {
 public:
  Session() = default;

  Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo);

  virtual ~Session() override;

  virtual void OnDisconnect() override;

  const uint32_t GetUniqueNo() const {
    return m_uniqueNo;
  }

 private:
  uint32_t m_uniqueNo;
};
}  // namespace Stress
