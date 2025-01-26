#pragma once
#include "pch.h"
#include <Session/RecvContext/TCP_RecvContextImpl.h>

namespace sh::IO_Engine {
int32_t TCP_RecvContextImpl::RecvCompletion(size_t ioSize) {
  int remainSize = ioSize + m_remainLen;
  BYTE* bufferPosition = m_buffer;

  while (remainSize > sizeof(PacketHeader::size)) {
    PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(bufferPosition);
    if (currentPacket->size > remainSize) {
      // �ϼ��� ��Ŷ�� ��������� ����.
      break;
    }
    // �ϼ��� ��Ŷ
    m_recvHandler(currentPacket->size, bufferPosition);
    // ���� �۹� ũ�� �ֽ�ȭ, ���� ���� ��ġ ���� ��Ŷ ���� ��ġ��
    remainSize -= currentPacket->size;
    bufferPosition = bufferPosition + currentPacket->size;
  }

  // ���� ���� ������ ũ�� ����
  m_remainLen = remainSize;
  // ���� ��Ŷ �����Ͱ� �ִٸ�, �� ������ ����
  if (remainSize > 0) {
    std::memcpy(m_buffer, bufferPosition, remainSize);
  }

  return DoRecv();
}

int32_t TCP_RecvContextImpl::DoRecv() {
  // wsaBuf�� buf ��ġ�� �ٲ�
  m_wsaBuf.buf = reinterpret_cast<char*>(m_buffer) + m_remainLen;
  m_wsaBuf.len = m_remainLen;
  DWORD flag = 0;
  WSARecv(m_socket, &m_wsaBuf, 1, nullptr, &flag, nullptr, nullptr);
  return 0;
}
}  // namespace sh::IO_Engine
