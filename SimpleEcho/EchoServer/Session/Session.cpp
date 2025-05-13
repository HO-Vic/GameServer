#include "pch.h"
#include "Session.h"
#include "SessionManager.h"

namespace sh::EchoServer {
Session::Session()
    : TCP_ISession() {
}
Session::~Session() {
}

void Session::OnDisconnect() {
  // Disconnect 됐을 때, 행동을 정의
  SessionManager::GetInstance().OnDisconnect(m_uniqueNo);
}
}  // namespace sh::EchoServer
