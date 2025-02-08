#pragma once

namespace sh {
namespace IO_Engine {
class IRecvContextImpl;
class OverlappedEx;
class IOverlappedEvent;
using OverlappedPtr = std::shared_ptr<IOverlappedEvent>;

class RecvContext {
 public:
  RecvContext(const IO_TYPE ioType, SOCKET sock, RecvHandler recvHandler);

  virtual ~RecvContext();

  int32_t RecvComplete(OverlappedEx* overlappedEx, size_t ioSize);

  int32_t StartRecv(OverlappedPtr& sesssion);

 private:
  IRecvContextImpl* m_recvContextImpl;
};
}
}  // namespace sh::IO_Engine
