#pragma once
#include <pch.h>

namespace sh {
namespace IO_Engine {
class IRecvContextImpl;

class RecvContext {
 public:
  RecvContext(const IO_TYPE ioType, SOCKET sock, RecvHandler recvHandler);

  virtual ~RecvContext();

  int32_t RecvCompletion(size_t ioSize);

  int32_t DoRecv();

 private:
  IRecvContextImpl* m_recvContextImpl;
};
}
}  // namespace sh::IO_Engine
