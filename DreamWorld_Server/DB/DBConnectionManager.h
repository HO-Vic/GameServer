#pragma once
#include <mutex>
#include <stack>
#include <memory>
#include "Utility/SingletonBase/Singleton.h"
#include "DBConnection.h"

// 커넥션은 DB 쓰레드 풀 갯수와 같아야 됨
// 커넥션은 무조건 유일성 가지게(shared xx)
namespace DreamWorld {
// class DBConnection;
using DBConnectionPtr = std::unique_ptr<DBConnection>;

class ConnectionGuard {
 public:
  ConnectionGuard(DBConnectionPtr&& connection);

  ConnectionGuard(const ConnectionGuard&) = delete;

  ~ConnectionGuard();

  DBConnection* operator->();

 private:
  DBConnectionPtr m_connection;
};

class DBConnectionManager : public sh::Utility::SingletonBase<DBConnectionManager> {
 public:
  ~DBConnectionManager();

  void Init(const uint8_t dbConnectionNo, const std::string& DBName, const std::string& dbAddr, const std::string& dbPort, const std::string& dbId, const std::string& dbpw);

  // RAII구조로 메모리 해지 명시안해도 할 수있게(오브젝트 풀로 들어감)
  ConnectionGuard GetConnection();

  void Release(DBConnectionPtr&& connection);

 private:
  std::mutex m_lock;
  std::stack<DBConnectionPtr> m_connections;

  // ODBC핸들 - SQLHANDLE
  // 환경 핸들 - 얘는 프로세스당 1개만 할당하면 되기 때문에, 매니져로 수정
  SQLHENV m_henv;
};
}  // namespace DreamWorld
