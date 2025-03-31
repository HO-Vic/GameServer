#include "stdafx.h"
#include "DBConnection.h"
#include <Windows.h>
#include <cassert>
#include <sqltypes.h>
#include <sqlext.h>
#include "../LogManager/LogManager.h"
#include "../DreamWorldDefine.h"

namespace DreamWorld {

DBConnection::DBConnection() {
}

DBConnection::~DBConnection() {
  SQLFreeConnect(m_hdbc);
}

bool DBConnection::Connect(SQLHENV env, const std::string& DBName, const std::string& dbAddr, const std::string& dbPort, const std::string& dbId, const std::string& dbPw) {
  SQLRETURN retCode;
  // SQLAllocHandle() - ODBC 핸들 변수 할당
  retCode = SQLAllocHandle(SQL_HANDLE_DBC, env, &m_hdbc);
  if (SQL_SUCCESS == retCode) {
    // Set login timeout to 5 second
    retCode = SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)(intptr_t)1, 0);
    if (SQL_SUCCESS == retCode) {
      // Connect to data source
      // SQL 연결, dbc, ODBC, 사용자 이름, 비밀번호...
      std::string addr = "SERVER=" + dbAddr + "," + dbPort + ";";
      std::string name = "DATABASE=" + DBName + ";";
      std::string id = "UID=" + dbId + ";";
      std::string pw = "PWD=" + dbPw + ";";
      std::wstring connStr = L"DRIVER={ODBC Driver 17 for SQL Server};";
      connStr += ConvertStringToWideString(addr.c_str());
      connStr += ConvertStringToWideString(name.c_str());
      connStr += ConvertStringToWideString(id.c_str());
      connStr += ConvertStringToWideString(pw.c_str());

      // SQLWCHAR connStr[] =
      //     L"DRIVER={ODBC Driver 17 for SQL Server};"
      //     L"SERVER=127.0.0.1,1433;"
      //     L"DATABASE=Dream_World;"  // ms sql은 하나의 host에서 여러 database를 가짐, mysql은 하나의 host 여러 스키마여서 헷갈림+mysql은 이게 기본 스키마
      //     L"UID=test;"
      //     L"PWD=test;";
      //  윈도우로 로그인이기때문에, id,pw없이
      ;
      retCode = SQLDriverConnect(m_hdbc, NULL, (SQLWCHAR*)connStr.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
      // retCode = SQLConnect(m_hdbc, (SQLWCHAR*)L"Dream_World_DB", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);
      if (SQL_SUCCESS == retCode) {
        WRITE_LOG(spdlog::level::info, "{}({}) > DB Connect Success", __FUNCTION__, __LINE__);
        return true;
      } else {
        if (SQL_SUCCESS_WITH_INFO == retCode) {
          WRITE_LOG(spdlog::level::info, "{}({}) > DB Connect Success With Info", __FUNCTION__, __LINE__);
          ErrorPrint(LogCallerInfo{
                         .funcInfo = __FUNCTION__,
                         .commonInfo = "DB Connect Success With Info",
                         .line = __LINE__,
                         .logLevel = spdlog::level::info},
                     SQL_HANDLE_DBC, m_hdbc);
          return true;
        } else {
          ErrorPrint(LogCallerInfo{
                         .funcInfo = __FUNCTION__,
                         .commonInfo = "DB Connect Fail",
                         .line = __LINE__,
                         .logLevel = spdlog::level::critical},
                     SQL_HANDLE_DBC, m_hdbc);
          assert(false && "Can Not Connect DB");
          return false;
        }
      }
    } else {
      ErrorPrint(LogCallerInfo{
                     .funcInfo = __FUNCTION__,
                     .commonInfo = "ODBC Set Attr Error Fail",
                     .line = __LINE__,
                     .logLevel = spdlog::level::critical},
                 SQL_HANDLE_DBC, m_hdbc);
      assert(false && "Can Not Connect DB");
      return false;
    }
  } else {
    ErrorPrint(LogCallerInfo{
                   .funcInfo = __FUNCTION__,
                   .commonInfo = "ODBC Alloc Error Fail",
                   .line = __LINE__,
                   .logLevel = spdlog::level::critical},
               SQL_HANDLE_DBC, m_hdbc);
    assert(false && "Can Not Connect DB");
    return false;
  }
  return false;
}
void DBConnection::ErrorPrint(LogCallerInfo& callerInfo, const SQLSMALLINT& handleType, SQLHANDLE handle) {
  // 오류 저장 버퍼
  SQLWCHAR sqlState[SQL_SQLSTATE_SIZE + 1]{0};
  SQLWCHAR sqlMessage[SQL_MAX_MESSAGE_LENGTH + 1]{0};
  SQLINTEGER sqlNativeError = 0;
  SQLSMALLINT sqlErrorLength = 0;

  int recNumber = 1;
  while (true) {
    SQLRETURN sqlRetVal = SQLGetDiagRec(handleType, handle, recNumber, sqlState, &sqlNativeError, sqlMessage, SQL_MAX_MESSAGE_LENGTH + 1, &sqlErrorLength);
    switch (sqlRetVal) {
      case SQL_SUCCESS:  // 진단 정보 존재
      {
        WRITE_LOG(callerInfo.logLevel, "\n****Stack Trace****\n{}({}) > SQL State: {}, SQL Error MSG: {}\n{}({}) > {}", __FUNCTION__, __LINE__, ConvertWideStringToString(sqlState), ConvertWideStringToString(sqlMessage),
                  callerInfo.funcInfo, callerInfo.line, callerInfo.commonInfo);
        ZeroMemory(sqlState, SQL_SQLSTATE_SIZE + 1);
        ZeroMemory(sqlMessage, sqlErrorLength + 1);
      } break;

      case SQL_SUCCESS_WITH_INFO:  // 버퍼가 작을 수 있음
      {
        WRITE_LOG(callerInfo.logLevel, "\n****Stack Trace****\n{}({}) > SQL State: {}, SQL Error MSG: {}, Small Buffer\n{}({}) > {}", __FUNCTION__, __LINE__, ConvertWideStringToString(sqlState), ConvertWideStringToString(sqlMessage),
                  callerInfo.funcInfo, callerInfo.line, callerInfo.commonInfo);
        // if (sqlErrorLength > SQL_MAX_MESSAGE_LENGTH) {
        //   spdlog::warn("DB::DBConnector::ErrorPrint() - SQL_SUCCESS_WITH_INFO");
        // }
        ZeroMemory(sqlState, SQL_SQLSTATE_SIZE + 1);
        ZeroMemory(sqlMessage, sqlErrorLength + 1);
      } break;

      case SQL_INVALID_HANDLE:  // 핸들 또는 핸들 타입이 잘못 됨.
      {
        WRITE_LOG(callerInfo.logLevel, "\n****Stack Trace****\n{}({}) > SQL Error MSG: SQL_INVALID_HANDLE\n{}({}) > {}", __FUNCTION__, __LINE__, ConvertWideStringToString(sqlState), ConvertWideStringToString(sqlMessage),
                  callerInfo.funcInfo, callerInfo.line, callerInfo.commonInfo);
      } break;

      case SQL_ERROR:  // recNumber <= 0 또는 bufferLength < 0 또는 비동기 알림을 사용하면 비동기 작업이 완료 되지 않음.
      {
        if (recNumber <= 0) {
          WRITE_LOG(callerInfo.logLevel, "\n****Stack Trace****\n{}({}) > SQL Error MSG: SQL_INVALID_HANDLE\n{}({}) > {}", __FUNCTION__, __LINE__, ConvertWideStringToString(sqlState), ConvertWideStringToString(sqlMessage),
                    callerInfo.funcInfo, callerInfo.line, callerInfo.commonInfo);
        }
        // bufferLength는 항상 존재 하기때문에 0미만이 되진 않음.
        // 비동기 작업을 수행하지 않음.
      } break;

      case SQL_NO_DATA:  // recNumber가 handle에 지정된 핸들에 대해 존재했던 진단 레코드 수 보다 큰 경우 또는
                         // Handle에 대한 진단 레코드가 없을 때 recNumber는 양수 + SQL_NO_DATA 반환
      {
        if (recNumber > 0) {
          return;
        }
      } break;
      default:
        break;
    }
    ++recNumber;
  }
}
}  // namespace DreamWorld
