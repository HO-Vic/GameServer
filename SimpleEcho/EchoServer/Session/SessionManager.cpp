#include "pch.h"
#include "SessionManager.h"

namespace sh::EchoServer {
sh::EchoServer::SessionManager::SessionManager()
    : Utility::ObjectPool<Session>(100) {
}
}  // namespace sh::EchoServer
