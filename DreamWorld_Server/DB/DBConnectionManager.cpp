#include "stdafx.h"
#include "DBConnectionManager.h"
#include "DBConnection.h"
#include "../LogManager/LogManager.h"

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

void DBConnectionManager::Init(const uint8_t dbConnectionNo) {
  WRITE_LOG(spdlog::level::info, "{}({})> Init!", __FUNCTION__, __LINE__);
  std::lock_guard lg{m_lock};
  for (uint8_t i = 0; i < dbConnectionNo; ++i) {
    m_connections.push(std::make_unique<DBConnection>());
    m_connections.top()->Connect();
  }
}

ConnectionGuard DBConnectionManager::GetConnection() {
  std::lock_guard lg{m_lock};
  if (m_connections.empty()) {
    return ConnectionGuard(nullptr);
  }
  auto conn = std::move(m_connections.top());
  m_connections.pop();
  return ConnectionGuard{std::move(conn)};
}

void DBConnectionManager::Release(DBConnectionPtr&& connection) {
  std::lock_guard lg{m_lock};
  m_connections.push(connection);
}
}  // namespace DreamWorld
