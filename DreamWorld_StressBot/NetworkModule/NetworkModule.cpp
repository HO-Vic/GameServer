#include "pch.h"
#include "NetworkModule.h"
#include <functional>
#include <optional>
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
void NetworkModule::Init(const std::string& ipAddr, uint16_t port, const uint8_t ioThreadNo, const uint64_t sustainedDelayMs, const uint64_t burstDelayMs, const uint64_t sustainedWindowMs, const uint64_t adjustConnectDelayThreadshold) {
  InitMsgDispatcher();
  g_connectUserCnt = 0;
  g_ActiveUserCnt = 0;
  g_maxConnectUserCnt = 0;
  m_ioCore.Init(ioThreadNo);
  m_connectDelayTick = MS(3);
  m_connector.Init(m_ioCore.GetHandle(), ipAddr, port, MS(0), AF_INET, SOCK_STREAM, IPPROTO_TCP);
  m_lastTryConnTime = TIME::now();
  m_sustainedDelayMs = sustainedDelayMs;
  m_burstDelayMs = burstDelayMs;
  m_sustainedWindow = MS(sustainedWindowMs);
  m_adjustConnectDelayThreadshold = adjustConnectDelayThreadshold;
}

void NetworkModule::Start() {
  m_ioCore.Start();
  Stress::SessionBatchUpdaters::GetInstance().Start();
  WRITE_LOG(logLevel::info, "{}({}) > Startr Network Module!!", __FUNCTION__, __LINE__);
  static constexpr MS DEC_USER_TICK = MS(50);
  static constexpr MS STATUS_LOG_TICK = MS(1000);
  bool isAdjustConnDelay = false;
  uint32_t adjustUserCnt = UINT32_MAX;  // 충원 기준선 (이 값 이하일 때만 connect 시도)
  bool cycleArmed = true;               // true면 다음 disconn 발동 시 새 사이클로 보고 1회 캡처
  auto decTime = TIME::now();
  auto statusLogTime = TIME::now();
  std::optional<TIME::time_point> overSustainedSince;
  while (true) {
    auto nowTime = TIME::now();

    // 충원 기준선 도달 → 다음 disconn 발동을 새 사이클로 인식 (= "받아들일 수 있는 순간")
    if (!cycleArmed && g_connectUserCnt <= adjustUserCnt) {
      cycleArmed = true;
    }

    if (g_connectUserCnt <= adjustUserCnt) {  // 정상 모드(adjustUserCnt=UINT32_MAX)이거나, 사이클 내 회복 기준선 이하로 떨어졌을 때
      auto connTimeDiff = std::chrono::duration_cast<MS>(nowTime - m_lastTryConnTime);
      if (connTimeDiff > m_connectDelayTick) {
        m_connector.TryConnect(std::bind(&NetworkModule::OnConnect, this, std::placeholders::_1), std::bind(&NetworkModule::OnConnectFail, this, std::placeholders::_1));
        m_lastTryConnTime = nowTime;
      }
    }
    if (0 != g_ActiveUserCnt) {
      g_avgDelay = g_totalDelay / g_ActiveUserCnt;

      // 1초 주기 상태 로그 (인원/평균지연/최대지연/충원기준선/사이클상태)
      if (std::chrono::duration_cast<MS>(nowTime - statusLogTime) >= STATUS_LOG_TICK) {
        statusLogTime = nowTime;
        WRITE_LOG(logLevel::info, "{}({}) Status [Active:{}, AvgDelay:{}ms, MaxDelay:{}ms, RecoverTarget:{}, CycleArmed:{}]",
                  __FUNCTION__, __LINE__,
                  g_ActiveUserCnt.load(),
                  g_avgDelay.load(),
                  g_maxDelayTime.load(),
                  adjustUserCnt,
                  cycleArmed ? 1 : 0);
      }

      // 1) Burst : 평균이 burst 임계 한 번이라도 넘으면 즉시 disconn
      // 2) Sustained : sustained 임계가 m_sustainedWindow 만큼 지속되어야 disconn
      const bool isBurst = g_avgDelay >= m_burstDelayMs;
      const bool isOverSustained = g_avgDelay >= m_sustainedDelayMs;

      if (isOverSustained) {
        if (!overSustainedSince) {
          overSustainedSince = nowTime;
        }
      } else {
        overSustainedSince.reset();  // 회복 시 카운터 리셋
      }

      const bool sustainedFired = overSustainedSince && (nowTime - *overSustainedSince) >= m_sustainedWindow;

      if (isBurst || sustainedFired) {
        auto connTimeDiff = std::chrono::duration_cast<MS>(nowTime - decTime);
        if (connTimeDiff < DEC_USER_TICK) {  // 이거 설정 안하면 우수수 다 나가버리는...
          continue;
        }
        decTime = nowTime;

        auto delSessionPtr = SessionManager::GetInstance().GetForceDiscardSession();
        if (nullptr == delSessionPtr) {
          continue;
        }
        auto uid = delSessionPtr->GetUniqueNo();
        //WRITE_LOG(logLevel::info, "{}({})> Force Disconn [uid:{}]", __FUNCTION__, __LINE__, uid);
        auto thWorkerJob = sh::IO_Engine::ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<sh::Utility::IWorkerItem>(delSessionPtr), sh::Utility::WORKER_TYPE::FORCE_DISCONN);
        PostQueuedCompletionStatus(m_ioCore.GetHandle(), 1, 0, static_cast<LPOVERLAPPED>(thWorkerJob));

        // 새 사이클의 첫 disconn 발동 시에만 충원 기준선 캡처
        if (cycleArmed) {
          adjustUserCnt = static_cast<uint32_t>(g_connectUserCnt.load() * 0.95);
          cycleArmed = false;
          WRITE_LOG(logLevel::info, "{}({}) Dec User Cycle Start [Avg Tick:{}, Reason:{}, RecoverTarget:{}]", __FUNCTION__, __LINE__, g_avgDelay.load(), isBurst ? "Burst" : "Sustained", adjustUserCnt);
        }
      }

      // 딜레이 평균이 충분히 크다면, connect Tick을 늘리자
      if (!isAdjustConnDelay && g_avgDelay >= m_adjustConnectDelayThreadshold) {
        isAdjustConnDelay = true;
        m_connectDelayTick = MS(15);
        WRITE_LOG(logLevel::info, "{}({}) Adjust Conn Tick!! [Avg Tick:{}]", __FUNCTION__, __LINE__, g_avgDelay.load());
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
  NetworkModule::GetInstance().g_maxConnectUserCnt++;

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

void NetworkModule::RecvHandle(sh::IO_Engine::TCP_SessionBasePtr sessionPtr, size_t n, BYTE* bufferPosition) {
  //WRITE_LOG(logLevel::info, "{}({})> Recv Size {}", __FUNCTION__, __LINE__, n);
  //if (n == 0) {
  //  WRITE_LOG(logLevel::info, "{}({})> Recv Size {}", __FUNCTION__, __LINE__, n);
  //  return;
  //}
  auto packetHeader = reinterpret_cast<DreamWorld::PacketHeader*>(bufferPosition);
  MsgHandler handler = nullptr;
  if (!m_msgDispatcher.GetHandler(packetHeader->type, handler)) {  // 없으면 에러 처리하지 않음, 스트레스 테스트에서 처리하지 않는게 많음
#ifdef _DEBUG
    if (packetHeader->type > static_cast<uint8_t>(DreamWorld::SERVER_PACKET::TYPE::STRESS_TEST_DELAY)) {
      return;
    }
#endif  // _DEBUG

    return;
  }
  handler(sessionPtr, bufferPosition);
}
}  // namespace Stress