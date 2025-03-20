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
};

class UserDBJobBase
    : public DBJobBase {
 protected:
  virtual void ExecuteFail() override;

 protected:
};

}  // namespace DreamWorld
