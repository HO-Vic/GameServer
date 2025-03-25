#include "stdafx.h"
#include "../ThreadManager/ThreadManager.h"
#include "Network/IOCP/Iocp.h"
#include "Timer/Timer.h"
#include "Network/UserSession/UserManager.h"
#include "Network/IocpEvent/IocpEventManager.h"
#include "DB/DB.h"
#include "Match/Matching.h"
#include "Room/RoomManager.h"
//
#include "Server/Server.h"
#include "DB/DBThreadPool.h"
#include "DB/DBConnectionManager.h"
#include "LogManager/LogManager.h"
#include "Room/RoomThreadPool.h"

int main() {
  std::wcout.imbue(std::locale("KOREAN"));
  // Trace – Debug – Info – Warning – Error – Critical
  START_LOGGER("GameServer", "Log/", "GameServer", static_cast<spdlog::level::level_enum>(0), "Console");
  // DreamWorld 객체 풀 하나 만들어서 하는게 좋을듯
  //  -> pool 모아둔 코드?로
  DreamWorld::Server server(0);
  server.Init();
  DreamWorld::DBConnectionManager::GetInstance().Init(1);
  DreamWorld::DBThreadPool::GetInstance().Init(1);
  DreamWorld::DBThreadPool::GetInstance().Start();

  DreamWorld::RoomThreadPool::GetInstance().Init(1);
  DreamWorld::RoomThreadPool::GetInstance().Start();

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
