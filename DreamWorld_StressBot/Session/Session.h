#pragma once
#include <Windows.h>
#include <winsock.h>
#include <IO_Engine/Session/ISession.h>
#include <IO_Engine/CommonDefine.h>
#include <Utility/Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>

/*
스트레스 테스트 세션
서버는 io-db-room(game) 3개의 쓰레드 그룹으로 이루어 졌다면
스트레스는 io-bot(update) 2개의 쓰레드 그룹

bot Update 쓰레드에서는 Update를 호출하여 행동 제어하자

io쓰레드에서 발생하는 이벤트는 jobQ로 넣고 업데이트 쓰레드에서 처리
*/

namespace Stress {
class Session
    : public sh::IO_Engine::ISession,
      public sh::Utility::DoubleJobQ_MT {
 public:
  Session() = default;

  Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo);

  virtual ~Session() override;

  virtual void OnDisconnect() override;

  const uint32_t GetUniqueNo() const {
    return m_uniqueNo;
  }

  void Update();

 private:
  uint32_t m_uniqueNo;
};
}  // namespace Stress
