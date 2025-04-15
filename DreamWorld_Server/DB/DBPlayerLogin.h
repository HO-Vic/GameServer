#pragma once
#include <sqltypes.h>
#include "DBJobBase.h"

namespace sh::IO_Engine {
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
constexpr short DB_NAME_SIZE = 20;
class DBPlayerLogin final
    : public UserDBJobBase {
 public:
  DBPlayerLogin() = default;

  DBPlayerLogin(sh::IO_Engine::ISessionPtr session, const char* loginId, const char* pw);

 private:
  void PostExecute(SQLHSTMT hstmt) override;
  std::wstring GetQuery() override;

 private:
  std::wstring m_id;
  std::wstring m_pw;
};
}  // namespace DreamWorld