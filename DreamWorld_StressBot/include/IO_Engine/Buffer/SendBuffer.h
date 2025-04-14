#pragma once
#include <minwindef.h>
#include <cstdint>

/*
Scatter-Gather
BroadCast -> shared_ptr<>::get()으로 raw ptr 참조 가능 + SendContext에서 ref 먹고 있으면 되긴 함
*/

namespace sh::IO_Engine {
constexpr static uint32_t MAX_SEND_BUFFER_SIZE = 1024;
class SendBuffer {
 public:
  SendBuffer() = default;
  SendBuffer(const BYTE* data, const size_t len);

 public:
  size_t m_size;
  BYTE m_buffer[MAX_SEND_BUFFER_SIZE];
};
}  // namespace sh::IO_Engine