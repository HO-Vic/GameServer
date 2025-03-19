#pragma once
#include <Windows.h>

namespace sh::Utility {
class ThWorkerJob;
class IWorkerItem : public std::enable_shared_from_this<IWorkerItem> {
 public:
  virtual void Execute(ThWorkerJob* workerJob, const DWORD ioByte) = 0;
};

using WokerPtr = std::shared_ptr<IWorkerItem>;
}  // namespace sh::Utility
