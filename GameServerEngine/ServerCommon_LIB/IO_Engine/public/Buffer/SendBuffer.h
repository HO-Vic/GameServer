#pragma once
#include <pch.h>

/*
Scatter-Gather
BroadCast -> shared_ptr<>::get()���� raw ptr ���� ���� + SendContext���� ref �԰� ������ �Ǳ� ��
*/

namespace sh::IO_Engine {
constexpr static uint32_t MAX_SEND_BUFFER_SIZE = 1024;
class SendBuffer {
 public:
  SendBuffer(const BYTE* data, const size_t len);

 public:
  size_t m_size;
  BYTE m_buffer[MAX_SEND_BUFFER_SIZE];
};
}  // namespace sh::IO_Engine