#include "stdafx.h"
#include "Server/Server.h"
#include "Match/Matching.h"
#include "DB/DBThreadPool.h"
#include "DB/DBConnectionManager.h"
#include "Room/RoomThreadPool.h"
#include "Room/RoomManager.h"
#include "ObjectPools.h"
#include "LogManager/LogManager.h"
#include "Match/Matching.h"
#include "Timer/Timer.h"
#include "Server/ServerConfig.h"

int main() {
  std::wcout.imbue(std::locale("KOREAN"));

  DreamWorld::ServerConfig::GetInstance().LoadXML("../../config/ServerConfig.xml");
  auto& serverConfig = DreamWorld::ServerConfig::GetInstance();
  // Trace – Debug – Info – Warning – Error – Critical
  START_LOGGER("GameServer", "DreamWorldLog/", "GameServer", static_cast<DreamWorld::logLevel>(serverConfig.logLevel), serverConfig.logMode);

  DreamWorld::Server server(serverConfig.ioThreadNo, serverConfig.useIOMetric, serverConfig.useGameMetric);  // ioThread
  server.Init();

  DreamWorld::DBConnectionManager::GetInstance().Init(serverConfig.DBThreadNo, serverConfig.dbName, serverConfig.dbIp, serverConfig.dbPort, serverConfig.dbId, serverConfig.dbpw);  // DB Connector
  DreamWorld::DBThreadPool::GetInstance().Init(serverConfig.DBThreadNo);                                                                                                            // DB Thread
  DreamWorld::DBThreadPool::GetInstance().Start();

  DreamWorld::RoomThreadPool::GetInstance().Init(serverConfig.roomThreadNo);  // Room Thread
  DreamWorld::RoomThreadPool::GetInstance().Start();

  DreamWorld::InitPool();
  DreamWorld::RoomManager::GetInstance().Init();

  DreamWorld::Timer::GetInstance().Start(serverConfig.timerThreadNo);
  DreamWorld::Matching::GetInstance().StartMatching();
  server.Start();

  /*
  0. io<=>게임 분리 하기위해 라이브러리 분리해서 적용 목적
  1. 문제점
                  a. Room업데이트는 io Thread pool에서 하는데, 어떻게 할지 생각 지금은 타이머 + PQGS()
                      i. Room만 업데이트하는 쓰레드 풀 생성
                           ii. Room을 묶어서 업데이트
                  b. DB결과도 마찬가지로 쿼리 하고나서 PQGS()
                            i. DB이벤트 자체에 type생성하면 문제x
  */
}
