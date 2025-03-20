#pragma once
#include <sqlext.h>
#include <sqltypes.h>

namespace DreamWorld {
class DBConnection {
 public:
  struct CallerInfo {
    const char* funcInfo = nullptr;
    const char* commonInfo = nullptr;
    uint16_t line = 0;
  };

  struct LogCallerInfo : public CallerInfo {
    const char* funcInfo = nullptr;
    const char* commonInfo = nullptr;
    uint16_t line = 0;
    spdlog::level::level_enum logLevel;
  };

 public:
  DBConnection();

  void Connect();

  SQLHDBC GetConection() const {
    return m_hdbc;
  }

  static void ErrorPrint(LogCallerInfo& callerInfo, const SQLSMALLINT& handleType, SQLHANDLE handle);

 private:
  // ODBC핸들 - SQLHANDLE
  // 환경 핸들
  SQLHENV m_henv;
  // 연결 핸들 DB Connection핸들
  SQLHDBC m_hdbc;

  // 명령 핸들 statement 핸들
  // SQLHSTMT m_hstmt;
};
}  // namespace DreamWorld
