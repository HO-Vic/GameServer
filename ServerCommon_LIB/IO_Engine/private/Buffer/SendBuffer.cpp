#include <pch.h>
#include <Buffer/SendBuffer.h>

namespace sh::IO_Engine {
SendBuffer::SendBuffer(const BYTE* data, const size_t len)
    : m_size(len) {
  memcpy_s(m_buffer, MAX_SEND_BUFFER_SIZE, data, m_size);
}
}  // namespace sh::IO_Engine