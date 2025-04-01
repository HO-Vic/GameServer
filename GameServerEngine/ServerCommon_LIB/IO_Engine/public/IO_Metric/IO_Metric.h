#pragma once
#include <Windows.h>
#include <atomic>
#include <Utility/SingletonBase/Singleton.h>

namespace sh::IO_Engine {
struct IO_Metric {
  std::atomic_uint32_t sendCompletion = 0;
  std::atomic_uint32_t recvCompletion = 0;

  std::atomic_uint64_t sendByte = 0;
  std::atomic_uint64_t recvByte = 0;

  std::atomic_uint64_t disconn = 0;
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

class IO_MetricSlot final
    : public sh::Utility::SingletonBase<IO_MetricSlot> {
 public:
  void Init(bool isUse = false);

  bool IsUse() const {
    return m_isUse;
  }

  IO_Metric& SwapAndLoad();

  void RecordSend(const DWORD ioByte);

  void RecordRecv(const DWORD ioByte);

  void RecordDisconn();

 private:
  IO_Metric m_metics[2];
  std::atomic_uint8_t m_index = 0;
  bool m_isUse = false;
};
}  // namespace sh::IO_Engine
