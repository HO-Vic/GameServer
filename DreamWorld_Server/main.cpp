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
  START_LOGGER("GameServer", "DreamWorldLog/", "GameServer", static_cast<DreamWorld::logLevel>(serverConfig.logLevel), serverConfig.logMode);
  WRITE_LOG(DreamWorld::logLevel::info, "Thread Pool Info [ioThread: {}] [roomThread: {}] [DBThread: {}]", serverConfig.ioThreadNo, serverConfig.roomThreadNo, serverConfig.DBThreadNo);
  // Trace – Debug – Info – Warning – Error – Critical

  DreamWorld::Server server(serverConfig.ioThreadNo, serverConfig.useIOMetric, serverConfig.useGameMetric);  // ioThread
  server.Init(serverConfig.ioThreadNo, serverConfig.thWorkerPoolSize, serverConfig.sendBufferPoolSize);
  DreamWorld::DBConnectionManager::GetInstance().Init(serverConfig.DBThreadNo, serverConfig.dbName, serverConfig.dbIp, serverConfig.dbPort, serverConfig.dbId, serverConfig.dbpw);  // DB Connector
  DreamWorld::DBThreadPool::GetInstance().Init(serverConfig.DBThreadNo);                                                                                                            // DB Thread
  DreamWorld::DBThreadPool::GetInstance().Start();

  DreamWorld::RoomThreadPool::GetInstance().Init(serverConfig.roomThreadNo);  // Room Thread
  DreamWorld::RoomThreadPool::GetInstance().Start();

  DreamWorld::InitPool();
  DreamWorld::RoomManager::GetInstance().Init();

  DreamWorld::Timer::GetInstance().Start(serverConfig.timerThreadNo);
  DreamWorld::Matching::GetInstance().Init(serverConfig.isLocal, serverConfig.testPeople);
  DreamWorld::Matching::GetInstance().StartMatching();
  server.Start();
}
