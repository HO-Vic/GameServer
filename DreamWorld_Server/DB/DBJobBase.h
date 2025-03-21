#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include <sqltypes.h>

namespace sh::Utility {
class ThWorkerJob;
}

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
 protected:
  virtual void ExecuteFail() override;

 protected:
};

/*
  DB 쿼리 이후에, 인게임 반영 어떻게 할지 추후에 생각해야 됨
   - DreamWorld는 로그인밖에 없어서...
*/
}  // namespace DreamWorld
