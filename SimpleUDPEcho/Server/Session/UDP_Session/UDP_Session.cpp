#include "pch.h"
#include "UDP_Session.h"

namespace sh::UDP_Echo {
UDP_Session::UDP_Session(const sockaddr_in& toAddr)
    : IO_Engine::UDP_SessionBase(toAddr), m_lastPingTime(Time::now()) {
}

void UDP_Session::OnUDPSessionTimeout() {
}
void UDP_Session::TimeOutCheck(TimePoint current) {
  auto deltaTime = std::chrono::duration_cast<ms>(current - m_lastPingTime).count();
  if (deltaTime > 5000) {  // 타임 아웃을 5초로 설정
    Close();
    // OnUDPSessionTimeout();
  }
}
}  // namespace sh::UDP_Echo