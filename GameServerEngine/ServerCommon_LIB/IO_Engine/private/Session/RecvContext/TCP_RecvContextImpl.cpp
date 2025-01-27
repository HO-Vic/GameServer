#pragma once
#include "pch.h"
#include <Session/RecvContext/TCP_RecvContextImpl.h>

namespace sh::IO_Engine {
int32_t TCP_RecvContextImpl::RecvComplete(OverlappedEx* overlappedEx, size_t ioSize) {
  size_t remainSize = ioSize + m_remainLen;
  BYTE* bufferPosition = m_buffer;

  while (remainSize > sizeof(PacketHeader::size)) {
    PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(bufferPosition);
    if (currentPacket->size > remainSize) {
      // 완성된 패킷이 만들어지지 않음.
      break;
    }
    // 완성된 패킷
    m_recvHandler(currentPacket->size, bufferPosition);
    // 남은 퍼버 크기 최신화, 현재 버퍼 위치 다음 패킷 시작 위치로
    remainSize -= currentPacket->size;
    bufferPosition = bufferPosition + currentPacket->size;
  }

  // 현재 남은 데이터 크기 저장
  m_remainLen = remainSize;
  // 남은 패킷 데이터가 있다면, 맨 앞으로 당기기
  if (remainSize > 0) {
    std::memcpy(m_buffer, bufferPosition, remainSize);
  }

  return DoRecv(overlappedEx);
}

int32_t TCP_RecvContextImpl::DoRecv(OverlappedEx* overlappedEx) {
  // wsaBuf의 buf 위치를 바꿈
  m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer) + m_remainLen;
  m_wsaBuf.len = static_cast<uint32_t>(m_remainLen);
  DWORD flag = 0;
  WSARecv(m_socket, &m_wsaBuf, 1, nullptr, &flag, reinterpret_cast<LPOVERLAPPED>(overlappedEx), nullptr);
  return 0;
}
}  // namespace sh::IO_Engine
