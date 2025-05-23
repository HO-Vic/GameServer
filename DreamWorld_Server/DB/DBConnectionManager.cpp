#include "stdafx.h"
#include "DBConnectionManager.h"
#include <sqlext.h>
#include <assert.h>
#include <memory>
#include "DBConnection.h"
#include "../LogManager/LogManager.h"
#include "../Server/ServerConfig.h"

namespace DreamWorld {
ConnectionGuard::ConnectionGuard(DBConnectionPtr&& connection)
    : m_connection(std::move(connection)) {
}

ConnectionGuard::~ConnectionGuard() {
  if (nullptr != m_connection) {
    DBConnectionManager::GetInstance().Release(std::move(m_connection));
  }
}

DBConnection* ConnectionGuard::operator->() {
  return m_connection.get();
}

DBConnectionManager::~DBConnectionManager() {
  SQLFreeEnv(m_henv);
}

void DBConnectionManager::Init(const uint8_t dbConnectionNo, const std::string& DBName, const std::string& dbAddr, const std::string& dbPort, const std::string& dbId, const std::string& dbpw) {
  WRITE_LOG(spdlog::level::info, "{}({})> Init!", __FUNCTION__, __LINE__);
  SQLRETURN retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
  // Set the ODBC version environment attribute
  if (SQL_SUCCESS == retCode) {
    // env설정
    retCode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

    // Allocate connection handle
    if (SQL_SUCCESS == retCode) {
      // dbc 할당
    } else {
      DBConnection::ErrorPrint(DBConnection::LogCallerInfo{
                                   .funcInfo = __FUNCTION__,
                                   .commonInfo = "Env Set Attr Error Fail",
                                   .line = __LINE__,
                                   .logLevel = spdlog::level::critical},
                               SQL_HANDLE_DBC, m_henv);
      assert("Can Not Connect DB");
    }
  } else {
    DBConnection::ErrorPrint(DBConnection::LogCallerInfo{
                                 .funcInfo = __FUNCTION__,
                                 .commonInfo = "Env Alloc Error Fail",
                                 .line = __LINE__,
                                 .logLevel = spdlog::level::critical},
                             SQL_HANDLE_DBC, m_henv);
    assert("Can Not Connect DB");
  }

  bool allConnected = true;
  std::lock_guard lg{m_lock};
  for (uint8_t i = 0; i < dbConnectionNo; ++i) {
    m_connections.emplace(std::make_unique<DBConnection>());
    allConnected = allConnected && m_connections.top()->Connect(m_henv, DBName, dbAddr, dbPort, dbId, dbpw);
  }
  if (!allConnected) {
    WRITE_LOG(logLevel::critical, "{}({}) > Specific Connection Raised Error", __FUNCTION__, __LINE__);
    assert(false && "Specific Connection Raised Error");
  }
}

ConnectionGuard DBConnectionManager::GetConnection() {
  {
    std::lock_guard lg{m_lock};
    if (!m_connections.empty()) {
      auto conn = std::move(m_connections.top());
      m_connections.pop();
      return ConnectionGuard{std::move(conn)};
    }
  }
  WRITE_LOG(logLevel::warn, "{}({}) > Non Exist Connection In Pool, Create New Connection", __FUNCTION__, __LINE__);
  auto newConn = std::make_unique<DBConnection>();
  newConn->Connect(m_henv,
                   ServerConfig::GetInstance().dbName,
                   ServerConfig::GetInstance().dbIp,
                   ServerConfig::GetInstance().dbPort,
                   ServerConfig::GetInstance().dbId,
                   ServerConfig::GetInstance().dbpw);
  return ConnectionGuard{std::move(newConn)};
}

void DBConnectionManager::Release(DBConnectionPtr&& connection) {
  std::lock_guard lg{m_lock};
  m_connections.push(std::move(connection));
}
}  // namespace DreamWorld
