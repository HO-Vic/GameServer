#pragma once
#include <WinSock2.h>

namespace sh {
namespace Utility {
class ThWorkerJob;
class IWorkerItem;
using WorkerPtr = std::shared_ptr<IWorkerItem>;
}
}  // namespace sh::Utility

namespace sh::IO_Engine {
class ISendContextImpl {
 public:
  ISendContextImpl(SOCKET sock)
      : m_socket(sock) {
  }

  virtual int32_t DoSend(Utility::WorkerPtr& session, const BYTE* data, const size_t len) = 0;

  virtual int32_t SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) = 0;

 protected:
  virtual int32_t SendExecute(Utility::ThWorkerJob* thWorkerJob) = 0;

 protected:
  SOCKET m_socket;
};
}  // namespace sh::IO_Engine
