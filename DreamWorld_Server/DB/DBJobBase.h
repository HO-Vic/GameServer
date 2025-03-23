#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include <sqltypes.h>
#include <memory>
#include <string>

namespace sh::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
class DBJobBase
    : public sh::Utility::IWorkerItem {
 public:
  virtual void Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte) override;

 protected:
  virtual void Proccess(SQLHSTMT hstmt) = 0;

  virtual std::wstring GetQuery() = 0;
  virtual void ExecuteFail() = 0;

 private:
  void Clear(sh::Utility::ThWorkerJob* workerJob);
};

class UserDBJobBase
    : public DBJobBase {
 public:
  UserDBJobBase(sh::IO_Engine::ISessionPtr session)
      : m_session(session) {
  }

 protected:
  virtual void ExecuteFail() override;  // 실패했다면, 클라로 DB실패 메시지 보내기

 protected:
  sh::IO_Engine::ISessionPtr m_session;
};

/*
  DB 쿼리 이후에, 인게임 반영 어떻게 할지 추후에 생각해야 됨
   - DreamWorld는 로그인밖에 없어서...
*/
}  // namespace DreamWorld
