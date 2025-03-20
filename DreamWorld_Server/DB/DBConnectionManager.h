#pragma once
#include "Utility/SingletonBase/Singleton.h"
#include <mutex>

// 커넥션은 DB 쓰레드 풀 갯수와 같아야 됨
// 커넥션은 무조건 유일성 가지게(shared xx)
namespace DreamWorld {
class DBConnection;
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
  void Init(const uint8_t dbConnectionNo);

  // RAII구조로 메모리 해지명시 하지 한더라도 할 수있게(오브젝트 풀로 들어감)
  ConnectionGuard GetConnection();

  void Release(DBConnectionPtr&& connection);

 private:
  std::mutex m_lock;
  std::stack<DBConnectionPtr> m_connections;
};
}  // namespace DreamWorld
