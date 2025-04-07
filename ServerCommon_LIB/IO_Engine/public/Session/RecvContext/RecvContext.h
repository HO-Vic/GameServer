#pragma once

namespace sh {
namespace Utility {
class ThWorkerJob;
class IWorkerItem;
using WorkerPtr = std::shared_ptr<IWorkerItem>;
}
}  // namespace sh::Utility

namespace sh {
namespace IO_Engine {
class IRecvContextImpl;

class RecvContext {
 public:
  RecvContext(const IO_TYPE ioType, SOCKET sock, RecvHandler recvHandler);

  virtual ~RecvContext();

  int32_t RecvComplete(Utility::ThWorkerJob* thWorkerJob, DWORD ioSize);

  int32_t StartRecv(Utility::WorkerPtr& sesssion);

 private:
  IRecvContextImpl* m_recvContextImpl;
};
}
}  // namespace sh::IO_Engine
