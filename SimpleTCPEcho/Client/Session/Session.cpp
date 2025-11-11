#include "pch.h"
#include "Session.h"
#include "../LogMgr/LogManager.h"

namespace SimpleTCP {
Session::Session(SOCKET sock, sh::IO_Engine::TCP_RecvHandler recvHandler, HANDLE iocpHandle) {
}
Session ::~Session() {
}

void Session::OnDisconnect() {
  WRITE_LOG(LogLevel::debug, "{}({})> Disconnct", __FUNCTION__, __LINE__);
}

}  // namespace SimpleTCP