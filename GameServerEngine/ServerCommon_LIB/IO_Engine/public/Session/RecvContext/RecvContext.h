#pragma once
#include <pch.h>

namespace sh {
namespace IO_Engine {
class IRecvContextImpl;
class OverlappedEx;
class RecvContext {
 public:
  RecvContext(const IO_TYPE ioType, SOCKET sock, RecvHandler recvHandler);

  virtual ~RecvContext();

  int32_t RecvComplete(OverlappedEx* overlappedEx, size_t ioSize);

  int32_t StartRecv();

 private:
  IRecvContextImpl* m_recvContextImpl;
};
}
}  // namespace sh::IO_Engine
