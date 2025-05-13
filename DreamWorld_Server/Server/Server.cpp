#include "stdafx.h"
#include "Server.h"
#include <functional>
#include <string>
#include <spdlog/common.h>
#include <IO_Engine/Session/TCP_ISession.h>
#include <IO_Engine/IO_Metric/IO_Metric.h>
#include "../Network/Session/SessionMananger.h"
#include "../Network/Session/Session.h"
#include "../MsgDispatcher/MsgDispatcher.h"
#include "../Room/RoomMsgDispatcher.h"
#include "MsgProtocol.h"
#include "../LogManager/LogManager.h"
#include "../Room/RoomManager.h"
#include "ServerConfig.h"
#include "../Metric/Metric.h"
#include "../DB/DBPlayerLogin.h"
#include "../ObjectPools.h"
#include "../Match/Matching.h"

namespace DreamWorld {
using logLevel = spdlog::level::level_enum;
Server::Server(const uint8_t ioThreadNo /*= 0*/, const bool useIoMetric /* = false*/, const bool useMetric /* = false*/)
    : m_ioCore(ioThreadNo, useIoMetric), m_acceptorCnt(2) {
  MetricSlot::GetInstance().Init(useMetric);
}

void Server::Init(const uint8_t ioThreadNo /*= 0*/, const uint32_t thWorkerPoolSize /*= 1500*/, const uint32_t sendBufferPoolSize /*= 1500*/) {
  m_ioCore.Init(ioThreadNo, thWorkerPoolSize, sendBufferPoolSize);
  m_listener.Init(m_ioCore.GetHandle(), 9000, 0);
  m_ioCore;
  m_acceptor.Init(m_ioCore.GetHandle(), [&](SOCKET sock) { AcceptHandle(sock); }, m_acceptorCnt);

  SessionMananger::GetInstance().Init();

  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::LOGIN), std::bind(Server::OnLogin, std::placeholders::_1, std::placeholders::_2));
  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MATCH), std::bind(Server::OnMatchReq, std::placeholders::_1, std::placeholders::_2));
  m_dispatcher.AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::STRESS_TEST_DELAY), std::bind(Server::OnStressDelay, std::placeholders::_1, std::placeholders::_2));

  WRITE_LOG(logLevel::info, "{}({}) > Room Msg Init!", __FUNCTION__, __LINE__);
  RoomMsgDispatcher::GetInstance().Init();
}

void Server::Start() {
  m_ioCore.Start();
  m_listener.Start();
  m_acceptor.SetListenSocket(m_listener.GetListenSocket());
  m_acceptor.Start();
  WRITE_LOG(logLevel::info, "{}({}) > Server Start!", __FUNCTION__, __LINE__);
  auto prevMetricLoggingTime = chrono_clock::now();
  auto& roomMgr = RoomManager::GetInstance();
  constexpr uint32_t loggingMaxUserCntThreshold = 4500;
  uint32_t maxActiveUserCnt = 0;
  while (true) {
    auto nowTime = chrono_clock::now();
    {  // IO_Metric
      auto loggingDiff = _chrono::duration_cast<_chrono::seconds>(nowTime - prevMetricLoggingTime);
      if (loggingDiff > ServerConfig::GetInstance().meticLoggingTickSec) {
        if (sh::IO_Engine::IO_MetricSlot::GetInstance().IsUse()) {
          auto& ioMetric = sh::IO_Engine::IO_MetricSlot::GetInstance().SwapAndLoad();
          auto sendCompletion = ioMetric.sendCompletion.load();
          auto sendByte = ioMetric.sendByte.load();
          auto recvCompletion = ioMetric.recvCompletion.load();
          auto recvByte = ioMetric.recvByte.load();
          auto disconn = ioMetric.disconn.load();
          auto thWorkerTotal = ioMetric.thWorkerJobTotal.load();
          auto thWorkerAdded = ioMetric.thWorkerJobAdd.load();
          auto thWorkerUsing = ioMetric.thWorkerJobUsing.load();
          auto SendBufferTotal = ioMetric.sendBufferTotal.load();
          auto SendBufferAdded = ioMetric.sendBufferAdd.load();
          auto SendBufferUsing = ioMetric.sendBufferUsing.load();

          WRITE_LOG(logLevel::info, "{}({}) > IO Metric [sendCompletion:{}] [sendByte:{}] [recvCompletion:{}] [recvByte:{}] [disconn:{}]", __FUNCTION__, __LINE__,
                    sendCompletion, sendByte, recvCompletion, recvByte, disconn);
          WRITE_LOG(logLevel::info, "{}({}) > IO Pool [thWorkerJobTotal:{}] [thWorkerJobAdd:{}] [thWorkerJobUsing:{}] [sendBufferTotal:{}] [sendBufferAdd:{}] [sendBufferUsing:{}]", __FUNCTION__, __LINE__,
                    thWorkerTotal, thWorkerAdded, thWorkerUsing, SendBufferTotal, SendBufferAdded, SendBufferUsing);
        }
        if (MetricSlot::GetInstance().IsUse()) {
          auto& gameMetric = MetricSlot::GetInstance().SwapAndLoad();
          auto roomExec = gameMetric.roomExec.load();
          auto DBExec = gameMetric.DBExec.load();
          auto timerExec = gameMetric.timerExec.load();
          auto timerAlready = gameMetric.timerAlreadyExec.load();
          auto timerIm = gameMetric.timerImmediate.load();

          auto timerJobTotal = gameMetric.timerJobTotalCnt.load();
          auto timerJobAdd = gameMetric.timerJobAddCnt.load();
          auto timerJobUsing = gameMetric.timerJobUsingCnt.load();

          auto jobTotal = gameMetric.jobTotalCnt.load();
          auto jobAdd = gameMetric.jobAddCnt.load();
          auto jobUsing = gameMetric.jobUsingCnt.load();

          auto roomTotal = gameMetric.roomTotalCnt.load();
          auto roomAdd = gameMetric.roomAddCnt.load();
          auto roomUsing = gameMetric.roomUsingCnt.load();

          auto playerLoginTotal = ObjectPool<DBPlayerLogin>::GetInstance().GetTotalCnt();
          auto playerLoginAdd = ObjectPool<DBPlayerLogin>::GetInstance().GetAddedCnt();
          auto playerLoginUsing = ObjectPool<DBPlayerLogin>::GetInstance().GetUsingCnt();

          auto warriorSize = Matching::GetInstance().GetWarriorSize();
          auto archerSize = Matching::GetInstance().GetArcherSize();
          auto tankerSize = Matching::GetInstance().GetTankerSize();
          auto mageSize = Matching::GetInstance().GetMageSize();

          auto userCnt = SessionMananger::GetInstance().GetCurrentActiveUserCnt();
          uint32_t roomCnt = roomMgr.globalRoomCnt;
          WRITE_LOG(logLevel::info, "{}({}) > Server Metric [ActiveUserCnt:{}] [ActiveroomCnt:{}] [roomExec:{}] [DBExec:{}] [timerExec:{}] [timerAlready:{}] [timerIm:{}]", __FUNCTION__, __LINE__,
                    userCnt, roomCnt, roomExec, DBExec, timerExec, timerAlready, timerIm);
          WRITE_LOG(logLevel::info, "{}({}) > Server Pool [jobTotal:{}] [jobAdd:{}] [jobUsing:{}] [timerJobTotal:{}] [timerJobAdd:{}] [timerJobUsing:{}]", __FUNCTION__, __LINE__,
                    jobTotal, jobAdd, jobUsing, timerJobTotal, timerJobAdd, timerJobUsing);
          WRITE_LOG(logLevel::info, "{}({}) > Server Pool [roomTotal:{}] [roomAdd:{}] [roomUsing:{}]] [playerLoginTotal:{}] [playerLoginAdd:{}] [playerLoginUsing:{}] ", __FUNCTION__, __LINE__,
                    roomTotal, roomAdd, roomUsing, playerLoginTotal, playerLoginAdd, playerLoginUsing);
          WRITE_LOG(logLevel::info, "{}({}) > Server Poo [warriorSize:{}] [archerSize:{}]] [tankerSize:{}] [mageSize:{}]", __FUNCTION__, __LINE__,
                    warriorSize, archerSize, tankerSize, mageSize);
        }
        prevMetricLoggingTime = nowTime;
      }
    }
    {  // Room Metric Logging
      auto loggingDiff = _chrono::duration_cast<_chrono::seconds>(nowTime - roomMgr.m_prevLoggingTime);
      if (loggingDiff > ServerConfig::GetInstance().avgRoomLogTickSec) {
        uint64_t globalTick = roomMgr.globalAvgRoomTick;
        uint32_t roomCnt = roomMgr.globalRoomCnt;
        if (0 == roomCnt) {
          continue;
        }
        globalTick = globalTick / roomCnt;
        roomMgr.m_prevLoggingTime = nowTime;

        auto userCnt = SessionMananger::GetInstance().GetCurrentActiveUserCnt();
        WRITE_LOG(logLevel::info, "{}({}) > Room Update Tick Metric [UserCnt: {}] [AvgRoomTick:{}Ms]", __FUNCTION__, __LINE__, userCnt, globalTick);
        if (maxActiveUserCnt < userCnt) {
          maxActiveUserCnt = userCnt;
          if (maxActiveUserCnt >= loggingMaxUserCntThreshold) {
            WRITE_LOG(logLevel::info, "{}({}) > Max Active User!!! [MaxActiveUserCnt:{}]  [AvgRoomTick:{}Ms]", __FUNCTION__, __LINE__, maxActiveUserCnt, globalTick);
          }
        }
      }
    }
    Sleep(100);
  }
}

void Server::AcceptHandle(SOCKET sock) {
  SessionMananger::GetInstance().OnAccept(sock, sh::IO_Engine::TCP, std::bind(&Server::RecvHandle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), m_ioCore.GetHandle());
}

void Server::RecvHandle(sh::IO_Engine::TCP_ISessionPtr sessionPtr, size_t ioByte, BYTE* bufferPosition) {
  auto packetHeader = reinterpret_cast<PacketHeader*>(bufferPosition);
  MsgHandler handler = nullptr;
  if (!m_dispatcher.GetHandler(packetHeader->type, handler)) {
    // 플레이어가 인게임(룸)인지 확인하고 디스패치
    // auto userPtr = std::static_pointer_cast<DreamWorld::Sesssion>(sessionPtr);
    if (!RoomMsgDispatcher::GetInstance().GetHandler(packetHeader->type, handler)) {
      WRITE_LOG(logLevel::err, "{}({}) > Can not Find Msg!", __FUNCTION__, __LINE__);
      return;
    }
  }
  handler(sessionPtr, bufferPosition);
}
}  // namespace DreamWorld