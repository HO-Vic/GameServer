#include "stdafx.h"
#include "DBConnection.h"
#include <cassert>
#include <sqlext.h>
#include <sqltypes.h>
#include "../LogManager/LogManager.h"

namespace DreamWorld {

DBConnection::DBConnection() {
}

void DBConnection::Connect() {
  SQLRETURN retCode;
  // Allocate environment handle
  // SQLAllocHandle() - ODBC 핸들 변수 할당
  retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

  // Set the ODBC version environment attribute
  if (SQL_SUCCESS == retCode) {
    // env설정
    retCode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

    // Allocate connection handle
    if (SQL_SUCCESS == retCode) {
      // dbc 할당
      retCode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

      if (SQL_SUCCESS == retCode) {
        // Set login timeout to 5 second
        retCode = SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
        if (SQL_SUCCESS == retCode) {
          // Connect to data source
          // SQL 연결, dbc, DB이름, 사용자 이름, 비밀번호...
          retCode = SQLConnect(m_hdbc, (SQLWCHAR*)L"Dream_World_DB", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);
          if (SQL_SUCCESS == retCode) {
            WRITE_LOG(spdlog::level::info, "{}({}) > DB Connect Success", __FUNCTION__, __LINE__);
          } else {
            if (SQL_SUCCESS_WITH_INFO == retCode) {
              WRITE_LOG(spdlog::level::info, "{}({}) > DB Connect Success With Info", __FUNCTION__, __LINE__);
              ErrorPrint(LogCallerInfo{
                             .funcInfo = __FUNCTION__,
                             .commonInfo = "DB Connect Success With Info",
                             .line = __LINE__,
                             .logLevel = spdlog::level::info},
                         SQL_HANDLE_DBC, m_hdbc);
            } else {
              ErrorPrint(LogCallerInfo{
                             .funcInfo = __FUNCTION__,
                             .commonInfo = "DB Connect Fail",
                             .line = __LINE__,
                             .logLevel = spdlog::level::critical},
                         SQL_HANDLE_DBC, m_hdbc);
              assert("Can Not Connect DB");
            }
          }
        } else {
          ErrorPrint(LogCallerInfo{
                         .funcInfo = __FUNCTION__,
                         .commonInfo = "ODBC Set Attr Error Fail",
                         .line = __LINE__,
                         .logLevel = spdlog::level::critical},
                     SQL_HANDLE_DBC, m_hdbc);
          assert("Can Not Connect DB");
        }
      } else {
        ErrorPrint(LogCallerInfo{
                       .funcInfo = __FUNCTION__,
                       .commonInfo = "ODBC Alloc Error Fail",
                       .line = __LINE__,
                       .logLevel = spdlog::level::critical},
                   SQL_HANDLE_DBC, m_hdbc);
        assert("Can Not Connect DB");
      }
    } else {
      ErrorPrint(LogCallerInfo{
                     .funcInfo = __FUNCTION__,
                     .commonInfo = "Env Set Attr Error Fail",
                     .line = __LINE__,
                     .logLevel = spdlog::level::critical},
                 SQL_HANDLE_DBC, m_hdbc);
      assert("Can Not Connect DB");
    }
  } else {
    ErrorPrint(LogCallerInfo{
                   .funcInfo = __FUNCTION__,
                   .commonInfo = "Env Alloc Error Fail",
                   .line = __LINE__,
                   .logLevel = spdlog::level::critical},
               SQL_HANDLE_DBC, m_hdbc);
    assert("Can Not Connect DB");
  }
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
