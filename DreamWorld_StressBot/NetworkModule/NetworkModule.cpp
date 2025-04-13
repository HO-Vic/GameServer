#include "pch.h"
#include "NetworkModule.h"
#include <functional>
#include <ws2def.h>
#include <WinSock2.h>
#include <winsock.h>
#include <memory>
#include <Utility/Job/Job.h>
#include <Utility/Thread/IWorkerItem.h>
#include <IO_Engine/IO_Core/ThWorkerJobPool.h>
#include "../Session/SessionManager.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"
#include "../LogManager/LogManager.h"
#include "../Session/SessionBatchUpdaters.h"
#include "../Session/SessionBatchUpdater.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"

namespace Stress {
void NetworkModule::Init(const std::string& ipAddr, uint16_t port) {
  InitMsgDispatcher();
  g_connectUserCnt = 0;
  g_ActiveUserCnt = 0;
  m_ioCore.Init(4);
  m_connectDelayTick = MS(1);
  m_connector.Init(m_ioCore.GetHandle(), ipAddr, port, AF_INET, SOCK_STREAM, IPPROTO_TCP, MS(0));
  m_lastTryConnTime = TIME::now();
}

void NetworkModule::Start() {
  m_ioCore.Start();
  WRITE_LOG(logLevel::info, "{}({}) > Startr Network Module!!", __FUNCTION__, __LINE__);
  static constexpr uint64_t MAX_DELAY_THRESHOLD = 150;
  static constexpr uint64_t INC_DELAY_CONN_TICK_THRESHOLD = 80;
  bool incUser = true;
  bool isAdjustConnDelay = false;
  uint32_t adjustUserCnt = UINT32_MAX;
  while (true) {
    if (incUser || (!incUser && g_ActiveUserCnt <= adjustUserCnt)) {  // 1차적으로 disconn후에, 일정 인원까지는 다시 채우자
      auto nowTime = TIME::now();
      auto connTimeDiff = std::chrono::duration_cast<MS>(nowTime - m_lastTryConnTime);
      if (connTimeDiff > m_connectDelayTick) {
        m_connector.TryConnect(std::bind(&NetworkModule::OnConnect, this, std::placeholders::_1), std::bind(&NetworkModule::OnConnectFail, this, std::placeholders::_1));
        m_lastTryConnTime = nowTime;
      }
    }
    if (0 != g_ActiveUserCnt) {
      g_avgDelay = g_totalDelay / g_ActiveUserCnt;
      // 여기서 딜레이 평균 측정해서 임계치 넘었다면, disconn
      if (g_avgDelay >= MAX_DELAY_THRESHOLD) {
        auto delSessionPtr = SessionManager::GetInstance().GetForceDiscardSession();
        if (nullptr == delSessionPtr) {
          continue;
        }
        auto thWorkerJob = sh::IO_Engine::ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<sh::Utility::IWorkerItem>(delSessionPtr), sh::Utility::WORKER_TYPE::FORCE_DISCONN);
        PostQueuedCompletionStatus(m_ioCore.GetHandle(), 1, 0, static_cast<LPOVERLAPPED>(thWorkerJob));
        adjustUserCnt = g_ActiveUserCnt * 0.8;

        if (incUser) {
          incUser = false;
          WRITE_LOG(logLevel::info, "{}({}) Dec User", __FUNCTION__, __LINE__);
        }
      }

      // 딜레이 평균이 충분히 크다면, connect Tick을 늘리자
      if (!isAdjustConnDelay && g_avgDelay >= INC_DELAY_CONN_TICK_THRESHOLD) {
        isAdjustConnDelay = true;
        m_connectDelayTick = MS(10);
        WRITE_LOG(logLevel::info, "{}({}) Adjust Conn Tick!!", __FUNCTION__, __LINE__);
      }
    }
  }
}

void NetworkModule::OnConnect(SOCKET sock) {
  using namespace std::placeholders;
  auto sessionPtr = SessionManager::GetInstance().OnConnect(sock, sh::IO_Engine::IO_TYPE::TCP, m_ioCore.GetHandle(), std::bind(&NetworkModule::RecvHandle, this, _1, _2, _3));
  sessionPtr->Init();
  sessionPtr->StartRecv();
  uint32_t connectCnt = NetworkModule::GetInstance().g_connectUserCnt++;

  auto batchUpdater = SessionBatchUpdaters::GetInstance().GetBatchUpdater(sessionPtr->GetUniqueNo());
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([batchUpdater, sessionPtr]() {
    if (sessionPtr->IsDisconnected()) {
      return;
    }
    batchUpdater->InsertSession(sessionPtr);
  });
  batchUpdater->InsertJob(std::move(jobPtr));
}

void NetworkModule::OnConnectFail(int errorCode) {
  // WRITE_LOG(logLevel::debug, "{}({}) > Connect Fail!! [WSAErrorCode:{}]", __FUNCTION__, __LINE__, errorCode);
}

void NetworkModule::RecvHandle(sh::IO_Engine::ISessionPtr sessionPtr, size_t, BYTE* bufferPosition) {
  auto packetHeader = reinterpret_cast<DreamWorld::PacketHeader*>(bufferPosition);
  MsgHandler handler = nullptr;
  if (!m_msgDispatcher.GetHandler(packetHeader->type, handler)) {  // 없으면 에러 처리하지 않음, 스트레스 테스트에서 처리하지 않는게 많음
    return;
  }
  handler(sessionPtr, bufferPosition);
}
}  // namespace Stress