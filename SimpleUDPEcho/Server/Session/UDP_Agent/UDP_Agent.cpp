#include "pch.h"
#include "UDP_Agent.h"

namespace sh::UDP_Echo {
UDP_Agent::UDP_Agent(SOCKET sock, uint32_t receiverNo, uint16_t port)
    : IO_Engine::UDP_AgentBase(sock, receiverNo, port) {
}

UDP_Agent::~UDP_Agent() {
}

void UDP_Agent::OnDestroy() {
}

void UDP_Agent::ReleaseSocket(SOCKET socket) {
  closesocket(socket);
}
}  // namespace sh::UDP_Echo