#include "pch.h"
#include "Session.h"
#include <type_traits>
#include <Utility/Job/Job.h>
#include "SessionBatchUpdater.h"
#include "SessionBatchUpdaters.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"
#include "SessionManager.h"
#include "../StressFSM/StressFSM.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"
#include "../NetworkModule/NetworkModule.h"
#include "../LogManager/LogManager.h"

namespace Stress {
Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo)
    : ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo), sh::Utility::DoubleJobQ_MT("Stress User"), m_currentState(SESSION_STATE::LOGIN), m_role(ROLE::NONE_SELECT), m_lastCheckTime(chrono_clock::now()), m_posX(-12341251.0f), m_posZ(-2351235.0f), m_isSendAbleDelayCheck(true), m_prevDelayTime(0) {
}

Session ::~Session() {
  SessionManager::GetInstance().InsertRestUniqueNo(GetUniqueNo());
  if (SESSION_STATE::LOGIN != m_currentState) {
    auto activeCnt = NetworkModule::GetInstance().g_ActiveUserCnt--;
  }
  NetworkModule::GetInstance().g_totalDelay -= m_prevDelayTime;
}

void Session::Init() {
  auto selfPtr = std::static_pointer_cast<Session>(shared_from_this());
  m_states.try_emplace(SESSION_STATE::LOGIN, std::make_shared<LoginState>(selfPtr));
  m_states.try_emplace(SESSION_STATE::MATCH, std::make_shared<MatchState>(selfPtr));
  m_states.try_emplace(SESSION_STATE::INGAME, std::make_shared<InGameState>(selfPtr));
  m_currentState = SESSION_STATE::LOGIN;
  m_states[m_currentState]->EnterState();
  m_lastCheckTime = chrono_clock::now();
}

void Session::OnDisconnect() {
  auto batchUpdater = SessionBatchUpdaters::GetInstance().GetBatchUpdater(GetUniqueNo());
  auto selfPtr = std::static_pointer_cast<Session>(shared_from_this());
  SessionManager::GetInstance().OnDisconnect(selfPtr);
  auto connectCnt = NetworkModule::GetInstance().g_connectUserCnt--;
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([batchUpdater, selfPtr]() {
    batchUpdater->DiscardSession(selfPtr);
  });
  batchUpdater->InsertJob(std::move(jobPtr));
}

void Session::Update() {
  DoJobs();
  m_states[m_currentState]->UpdateState();
  DelayCheck();
}

void Session::ChangeState(const SESSION_STATE state) {
  m_states[m_currentState]->ExitState();
  m_currentState = state;
  m_states[m_currentState]->EnterState();
}

void Session::OnDelayResponse() {
  //// 여기서 딜레이 파악
  auto nowTime = chrono_clock::now();
  // 현재 시간과 보낸 시간의 차이를 계산
  auto durationTime = _chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastCheckTime);
  long long currentDelayTime = durationTime.count();  // currentDelay

  if (NetworkModule::GetInstance().g_maxDelayTime < currentDelayTime) {
    NetworkModule::GetInstance().g_maxDelayTime = currentDelayTime;
  }

  long long diffPrevDelay = currentDelayTime - m_prevDelayTime;  // 이전 딜레이와 현재 딜레이 차이

  NetworkModule::GetInstance().g_totalDelay += diffPrevDelay;
  m_prevDelayTime = currentDelayTime;
  m_isSendAbleDelayCheck = true;
}

void Session::DelayCheck() {
  auto nowTime = chrono_clock::now();
  if (m_isSendAbleDelayCheck && _chrono::duration_cast<_chrono::milliseconds>(nowTime - m_lastCheckTime).count() >= 1000) {
    m_isSendAbleDelayCheck = false;
    DreamWorld::CLIENT_PACKET::NotifyPacket sendPacket(static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::STRESS_TEST_DELAY));
    m_lastCheckTime = nowTime;
    DoSend(&sendPacket, sendPacket.size);
  }
}
}  // namespace Stress