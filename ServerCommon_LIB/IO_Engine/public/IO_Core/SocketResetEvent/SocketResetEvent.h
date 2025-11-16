#pragma once
#include <Utility/Thread/IWorkerItem.h>

/*
  TCP Socket Life Cycle:
    Create: AcceptEvent
    Use: TCP_SessionBase
    Destroy: ~TCP_SessionBase()
      └> Recycle Socket To AcceptEvent
*/

namespace sh {
namespace Utility {
class ThWorkerJob;
}  // namespace Utility

namespace IO_Engine {
class AcceptEvent;
class SocketResetEvent
    : public Utility::IWorkerItem {
 public:
  SocketResetEvent() = default;

  SocketResetEvent(std::shared_ptr<AcceptEvent>&& acceptEvent, SOCKET sock)
      : m_acceptEvent(std::move(acceptEvent)), m_recycleSocket(sock) {
  }

  bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

 private:
  std::shared_ptr<AcceptEvent> m_acceptEvent;
  SOCKET m_recycleSocket = NULL;
};
}  // namespace IO_Engine
}  // namespace sh
