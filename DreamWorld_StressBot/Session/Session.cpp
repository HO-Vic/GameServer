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

namespace Stress {
Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo)
    : ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo), sh::Utility::DoubleJobQ_MT("Stress User"), m_currentState(nullptr), m_role(ROLE::NONE_SELECT) {
}

Session ::~Session() {
  SessionManager::GetInstance().InsertRestUniqueNo(GetUniqueNo());
}

void Session::Init() {
  auto selfPtr = std::static_pointer_cast<Session>(shared_from_this());
  m_states.try_emplace(SESSION_STATE::LOGIN, std::make_shared<LoginState>(selfPtr));
  m_states.try_emplace(SESSION_STATE::MATCH, std::make_shared<MatchState>(selfPtr));
  m_states.try_emplace(SESSION_STATE::INGAME, std::make_shared<InGameState>(selfPtr));
  m_currentState = m_states[SESSION_STATE::LOGIN];
  m_currentState->EnterState();
  m_lastCheckTime = chrono_clock::now();
}

void Session::OnDisconnect() {
  auto batchUpdater = SessionBatchUpdaters::GetInstance().GetBatchUpdater(GetUniqueNo());
  auto selfPtr = std::static_pointer_cast<Session>(shared_from_this());
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([batchUpdater, selfPtr]() {
    batchUpdater->DiscardSession(selfPtr);
  });
  batchUpdater->InsertJob(std::move(jobPtr));
}

void Session::Update() {
  DoJobs();
  m_currentState->UpdateState();
  auto nowTime = chrono_clock::now();
  if (_chrono::duration_cast<_chrono::milliseconds>(nowTime - m_lastCheckTime).count() >= 1000) {
    DreamWorld::CLIENT_PACKET::NotifyPacket sendPacket(static_cast<char>(DreamWorld::CLIENT_PACKET::TYPE::STRESS_TEST_DELAY));
    DoSend(&sendPacket, sendPacket.size);
    m_lastCheckTime = nowTime;
  }
}

void Session::ChangeState(const SESSION_STATE state) {
  m_currentState->ExitState();
  m_currentState = m_states[state];
  m_currentState->EnterState();
}

}  // namespace Stress