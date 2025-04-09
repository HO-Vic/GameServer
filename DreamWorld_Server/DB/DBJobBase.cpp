#include "stdafx.h"
#include "DBJobBase.h"
#include <sqlext.h>
#include <spdlog/common.h>
#include <sqlucode.h>
#include "DBConnectionManager.h"
#include "DBConnection.h"
#include "Utility/Thread/ThWorkerJob.h"
#include "IO_Engine/IO_Core/ThWorkerJobPool.h"
#include "../Metric/Metric.h"

using logLevel = spdlog::level::level_enum;

namespace DreamWorld {
void DreamWorld::DBJobBase::Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
  auto connection = DBConnectionManager::GetInstance().GetConnection();

  SQLRETURN retCode;
  SQLHSTMT sqlStatement;
  // Allocate statement handle
  retCode = SQLAllocHandle(SQL_HANDLE_STMT, connection->GetConection(), &sqlStatement);
  if (SQL_SUCCESS != retCode) {
    DBConnection::ErrorPrint(DBConnection::LogCallerInfo{
                                 .funcInfo = __FUNCTION__,
                                 .commonInfo = "STMT Alloc Error",
                                 .line = __LINE__,
                                 .logLevel = logLevel::warn},
                             SQL_HANDLE_STMT, sqlStatement);
    if (SQL_ERROR == retCode) {
      ExecuteFail();
      SQLCancel(sqlStatement);                       /// 종료
      SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);  // 리소스 해제
      Clear(workerJob);
      return;
    }
  }

  std::wstring query = GetQuery();
  // SQL문 실행 - stmt, 실행할 SQL문, 실행할 SQL문 길이
  retCode = SQLExecDirect(sqlStatement, (SQLWCHAR*)query.c_str(), static_cast<SQLINTEGER>(query.size()));

  if (SQL_ERROR == retCode || SQL_INVALID_HANDLE == retCode) {
    // 문제가 있다면 클라이언트에 알림
    DBConnection::ErrorPrint(DBConnection::LogCallerInfo{
                                 .funcInfo = __FUNCTION__,
                                 .commonInfo = "ExecDirect Alloc Error",
                                 .line = __LINE__,
                                 .logLevel = logLevel::warn},
                             SQL_HANDLE_STMT, sqlStatement);

    ExecuteFail();                                 // 클라이언트에 다시 시도 알림
    SQLCancel(sqlStatement);                       /// 종료
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);  // 리소스 해제
    Clear(workerJob);
    return;
  }
#ifdef _DEBUG
  else if (SQL_SUCCESS_WITH_INFO == retCode) {
    DBConnection::ErrorPrint(DBConnection::LogCallerInfo{
                                 .funcInfo = __FUNCTION__,
                                 .commonInfo = "SUCCESS WITH INFO",
                                 .line = __LINE__,
                                 .logLevel = logLevel::warn},
                             SQL_HANDLE_STMT, sqlStatement);
  }
#endif  // _DEBUG

  // retCode
  //  쿼리에 대한 결과 처리
  PostExecute(sqlStatement);

  // stmt 해제
  SQLCancel(sqlStatement);                       /// 종료
  SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);  // 리소스 해제
  Clear(workerJob);
}
void DBJobBase::Clear(sh::Utility::ThWorkerJob* workerJob) {
  MetricSlot::GetInstance().RecordDB();
  sh::IO_Engine::ThWorkerJobPool::GetInstance().Release(workerJob);
}

void UserDBJobBase::ExecuteFail() {
  // 하지만 DreamWorld 클라는 DB 실패에 대한 작업이 없기에 패스...ㅠ
  // m_session->DoSend(nullptr, 0);
}
}  // namespace DreamWorld
