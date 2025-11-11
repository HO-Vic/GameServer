#pragma once
#include <Windows.h>
#include <memory>
namespace sh::Utility {
class ThWorkerJob;
class IWorkerItem : public std::enable_shared_from_this<IWorkerItem> {
 public:
  virtual ~IWorkerItem() = default;
  virtual bool Execute(ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) = 0;
};

using WokerPtr = std::shared_ptr<IWorkerItem>;
}  // namespace sh::Utility
