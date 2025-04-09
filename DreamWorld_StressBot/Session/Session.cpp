#include "pch.h"
#include "Session.h"

namespace Stress {
Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, const uint32_t uniqueNo)
    : ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo) {
}

Session ::~Session() {
}

void Session::OnDisconnect() {
}
}  // namespace Stress