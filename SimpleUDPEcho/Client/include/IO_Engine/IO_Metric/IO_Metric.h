#pragma once
#include <Windows.h>
#include <atomic>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
struct IO_MetricSlot {
  std::atomic_uint64_t sendCompletion = 0;
  std::atomic_uint64_t recvCompletion = 0;

  std::atomic_uint64_t sendByte = 0;
  std::atomic_uint64_t recvByte = 0;

  std::atomic_uint64_t disconn = 0;

  std::atomic_uint64_t thWorkerJobTotal = 0;
  std::atomic_uint64_t thWorkerJobUsing = 0;
  std::atomic_uint64_t thWorkerJobAdd = 0;

  std::atomic_uint64_t smallSendBufferTotal = 0;
  std::atomic_uint64_t smallSendBufferUsing = 0;
  std::atomic_uint64_t smallSendBufferAdd = 0;

  std::atomic_uint64_t mediumSendBufferTotal = 0;
  std::atomic_uint64_t mediumSendBufferUsing = 0;
  std::atomic_uint64_t mediumSendBufferAdd = 0;

  std::atomic_uint64_t largeSendBufferTotal = 0;
  std::atomic_uint64_t largeSendBufferUsing = 0;
  std::atomic_uint64_t largeSendBufferAdd = 0;

  std::atomic_uint64_t xLargeSendBufferTotal = 0;
  std::atomic_uint64_t xLargeSendBufferUsing = 0;
  std::atomic_uint64_t xLargeSendBufferAdd = 0;

  std::atomic_uint64_t dynamicSendBufferUsing = 0;

  std::atomic_uint32_t totalReq = 0;

  void Reset() {
    sendCompletion = 0;
    recvCompletion = 0;
    sendByte = 0;
    recvByte = 0;
    disconn = 0;
    totalReq = 0;
  }
};

class IO_Metric final
    : public sh::Utility::SingletonBase<IO_Metric> {
 public:
  void Init(bool isUse = false);

  bool IsUse() const {
    return m_isUse;
  }

  IO_MetricSlot& SwapAndLoad();

  void RecordSend(const DWORD ioByte);

  void RecordRecv(const DWORD ioByte);

  void RecordDynamicSendBuffer();

  void RecordDisconn();

 private:
  IO_MetricSlot m_metics[2];
  std::atomic_uint8_t m_index = 0;
  bool m_isUse = false;
};
}  // namespace sh::IO_Engine
