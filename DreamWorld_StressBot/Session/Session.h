#pragma once
#include <Windows.h>
#include <winsock.h>
#include <unordered_map>
#include <chrono>
#include <IO_Engine/Session/TCP_ISession.h>
#include <IO_Engine/CommonDefine.h>
#include <Utility/Job/JobQueue/JobQ_MT/DoubleJobQ_MT.h>
#include "../../DreamWorld_Server/DreamWorldDefine.h"

/*
스트레스 테스트 세션
서버는 io-db-room(game) 3개의 쓰레드 그룹으로 이루어 졌다면
스트레스는 io-bot(update) 2개의 쓰레드 그룹

bot Update 쓰레드에서는 Update를 호출하여 행동 제어하자

io쓰레드에서 발생하는 이벤트는 jobQ로 넣고 업데이트 쓰레드에서 처리
*/

namespace Stress {
class StressFSMBase;
using FSMPtr = std::shared_ptr<StressFSMBase>;

class Session final
    : public sh::IO_Engine::TCP_ISession,
      public sh::Utility::DoubleJobQ_MT {
 public:
  enum class SESSION_STATE : char {
    LOGIN = 1,
    MATCH = 2,
    INGAME
  };
  Session() = default;

  Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo);

  ~Session() override;

  void Init();

  void OnDisconnect() override;

  const uint32_t GetUniqueNo() const {
    return m_uniqueNo;
  }

  void Update();

  void ChangeState(const SESSION_STATE state);

  void SetRole(ROLE r) {
    m_role = r;
  }

  ROLE GetRole() const {
    return m_role;
  }

  std::pair<float, float> GetPosition() const {
    return {m_posX, m_posZ};
  }

  void SetPosition(float x, float z) {
    m_posX = x;
    m_posZ = z;
  }

  SESSION_STATE GetSessionState() const {
    return m_currentState;
  }

  void OnDelayResponse();

 private:
  void DelayCheck();

 private:
  SESSION_STATE m_currentState;
  std::unordered_map<SESSION_STATE, FSMPtr> m_states;

  // 인 게임 위치 (서버에서 받은 위치만 드로우 모듈에서 렌더링)
  float m_posX;
  float m_posZ;

  uint32_t m_uniqueNo;
  chrono_clock::time_point m_lastCheckTime;  // 딜레이 측정을 위한 시간입니다
  uint64_t m_prevDelayTime;
  volatile bool m_isSendAbleDelayCheck;
  ROLE m_role;
};
}  // namespace Stress
