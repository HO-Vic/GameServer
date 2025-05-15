#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include <memory>
#include <string>

namespace sh::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class TCP_ISession;
using TCP_ISessionPtr = std::shared_ptr<TCP_ISession>;
using TCP_ISessionWeakPtr = std::weak_ptr<TCP_ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
class DBJobBase
    : public sh::Utility::IWorkerItem {
 public:
  DBJobBase() = default;

  virtual bool Execute(sh::Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

 protected:
  // 쿼리 결과 처리
  virtual void PostExecute(SQLHSTMT hstmt) = 0;

  virtual std::wstring GetQuery() = 0;
  virtual void ExecuteFail() = 0;

 private:
  void Clear(sh::Utility::ThWorkerJob* workerJob);
};

class UserDBJobBase
    : public DBJobBase {
 public:
  UserDBJobBase() = default;

  UserDBJobBase(sh::IO_Engine::TCP_ISessionPtr session)
      : m_session(session) {
  }

 protected:
  virtual void ExecuteFail() override;  // 실패했다면, 클라로 DB실패 메시지 보내기

 protected:
  sh::IO_Engine::TCP_ISessionWeakPtr m_session;
};

using DBJobPtr = std::shared_ptr<DBJobBase>;
/*
  DB 쿼리 이후에, 인게임 반영 어떻게 할지 추후에 생각해야 됨
   - DreamWorld는 로그인밖에 없어서...
   게임 로직은 Room Thread Pool에서 돌기 때문에, 실제 인게임 메모리 반영이라면 Room에 Utility::Job으로 인서트해서 인게임 메모리 반영
*/
}  // namespace DreamWorld
