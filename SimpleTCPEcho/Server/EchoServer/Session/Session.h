#pragma once
#include <IO_Engine/CommonDefine.h>
#include <IO_Engine/Session/TCP_ISession.h>

namespace SimpleTCP {
class Session final
    : public sh::IO_Engine::TCP_ISession {
 public:
  Session();

  Session(SOCKET sock, sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::TCP_RecvHandler TCP_RecvHandler, HANDLE iocpHandle, uint32_t uniqueNo)
      : TCP_ISession(sock, ioType, TCP_RecvHandler, iocpHandle), m_uniqueNo(uniqueNo) {
  }

  virtual ~Session() override;

  virtual void OnDisconnect() override;

  const uint32_t GetUniqueNo() const {
    return m_uniqueNo;
  }

 private:
  uint32_t m_uniqueNo;
};
}  // namespace SimpleTCP
