#include "stdafx.h"
#include "DBPlayerLogin.h"
#include <format>
#include <string>
#include <sqlext.h>
#include <sqltypes.h>
#include "../DreamWorldDefine.h"
#include <IO_Engine/Session/ISession.h>
#include "../LogManager/LogManager.h"
#include "DBConnection.h"
#include "DBConnectionManager.h"
#include "../Server/MsgProtocol.h"

namespace DreamWorld {
DreamWorld::DBPlayerLogin::DBPlayerLogin(sh::IO_Engine::ISessionPtr session, const char* loginId, const char* pw)
    : UserDBJobBase(session) {
  m_id = ConvertStringToWideString(loginId);
  m_pw = ConvertStringToWideString(pw);
}

void DBPlayerLogin::PostExecute(SQLHSTMT hstmt) {
  auto sessionPtr = m_session.lock();
  if (nullptr == sessionPtr) {  // 이미 연결이 끊긴 유저
    return;
  }
  // 플레이어 정보가 존재
  SQLRETURN retCode;
  SQLWCHAR playerName[DB_NAME_SIZE] = {0};
  SQLLEN dataLength = SQL_NULL_DATA;
  // namelength
  // SQL_NULL_DATA
  // SQL_NO_TOTAL

  // BufferLegth는 wchar_t: 2바이트 이기때문에 * 2 계산
  retCode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, playerName, static_cast<SQLLEN>(DB_NAME_SIZE * 2), &dataLength);
  if (SQL_SUCCESS != retCode) {
    DBConnection::ErrorPrint(
        DBConnection::LogCallerInfo{
            .funcInfo = __FUNCTION__,
            .commonInfo = "SQLBindCol Raise Error",
            .line = __LINE__,
            .logLevel = logLevel::err,
        },
        SQL_HANDLE_STMT, hstmt);
  }
  retCode = SQLFetch(hstmt);  // fetch는 loop로 record 순회할 때 씀

  if (SQL_NO_DATA == retCode) {  // 플레이어 정보가 없음
    // stmt종료는 함수 외부에서 수행
    return;
  }

  else if (SQL_SUCCESS != retCode) {
    DBConnection::ErrorPrint(
        DBConnection::LogCallerInfo{
            .funcInfo = __FUNCTION__,
            .commonInfo = "SQLBindCol Raise Error",
            .line = __LINE__,
            .logLevel = logLevel::err,
        },
        SQL_HANDLE_STMT, hstmt);
    // DB 에러 보내기
    return;
  }
  DreamWorld::SERVER_PACKET::LoginPacket loginPacket{};
  lstrcpyn(loginPacket.nickName, playerName, DB_NAME_SIZE);
  sessionPtr->DoSend(&loginPacket, loginPacket.size);
  /*
          SQL_SUCCESS_WITH_INFO
          SQL_NEED_DATA - 아마 발생 안함
          SQL_STILL_EXECUTING - 비동기 일때 생기는 에러코드
  */
}

std::wstring DreamWorld::DBPlayerLogin::GetQuery() {
  return std::format(L"EXEC GET_PLAYER_INFO {},{}", m_id, m_pw);
}
}  // namespace DreamWorld
