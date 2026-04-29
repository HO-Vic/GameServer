#pragma once
#include <cstdint>
#include <atomic>
#include <chrono>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
struct RecvTimeRaii {
  RecvTimeRaii()
      : startTime(std::chrono::steady_clock::now()) {
  }
  ~RecvTimeRaii();
  std::chrono::steady_clock::time_point startTime;
};

struct MergeTimeRaii {
  MergeTimeRaii()
      : startTime(std::chrono::steady_clock::now()) {
  }
  ~MergeTimeRaii();
  std::chrono::steady_clock::time_point startTime;
};

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

  std::atomic_uint64_t recvElapsed = 0;
  std::atomic_uint64_t recvElasedCnt = 0;
  std::atomic_uint64_t recvMergeCnt = 0;
  std::atomic_uint64_t recvMergeSize = 0;
  std::atomic_uint64_t MergeElapsed = 0;

  void Reset() {
    sendCompletion = 0;
    recvCompletion = 0;
    sendByte = 0;
    recvByte = 0;
    disconn = 0;
    totalReq = 0;
    recvElapsed = 0;
    recvElasedCnt = 0;
    recvMergeCnt = 0;
    recvMergeSize = 0;
    MergeElapsed = 0;
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

  void RecordRecvElapsed(uint64_t time);

  void RecordMergeElapsed(uint64_t time);

  void RecordRecvMerge(uint64_t mergeByte);

 private:
  IO_MetricSlot m_metics[2];
  std::atomic_uint8_t m_index = 0;
  bool m_isUse = false;
};
}  // namespace sh::IO_Engine
