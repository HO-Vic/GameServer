#include "pch.h"
#include <Session/UDP_IAgent.h>
#include <Session/RecvContext/UDP_RecvContext.h>
#include <Utility/Thread/ThWorkerJob.h>

namespace sh::IO_Engine {
UDP_IAgent::UDP_IAgent(SOCKET sock, uint32_t receiverNo, uint16_t port)
    : m_socket(sock), m_activeReceiverCnt(receiverNo), m_port(port) {
}

UDP_IAgent::~UDP_IAgent() {
  ReleaseSocket(m_socket);
}

void UDP_IAgent::Destroy() {
  // 여기서 recv 버퍼 남은거 flush
}

void UDP_IAgent::StopReq() {
  // 해당 socket에 대해서 모든 이벤트 종료
  m_state = STATE::INACTIVE;
  CancelIoEx(reinterpret_cast<HANDLE>(m_socket), nullptr);
}

void UDP_IAgent::StartRecv() {
  auto cnt = m_activeReceiverCnt.load();
  for (auto i = 0; i < cnt; ++i) {
    // m_receiver.push_back();
  }
}

void UDP_IAgent::DestroyFromReceiver() {
  m_activeReceiverCnt--;
  if (0 == m_activeReceiverCnt) {
    Destroy();
  }
}
}  // namespace sh::IO_Engine
