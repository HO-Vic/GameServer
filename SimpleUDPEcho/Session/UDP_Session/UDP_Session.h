#pragma once
#include <IO_Engine/Session/UDP_ISession.h>
#include <chrono>

namespace sh::UDP_Echo {
using Time = std::chrono::steady_clock;
using TimePoint = Time::time_point;
using ms = std::chrono::milliseconds;
using sec = std::chrono::seconds;

class UDP_Session
    : public IO_Engine::UDP_ISession {
 public:
  UDP_Session() = default;

  UDP_Session(const sockaddr_in& toAddr);

  virtual void OnUDPSessionTimeout() override;  // 하트비트, 재전송 횟수 초과 시, 종료할 때 호출

  void TimeOutCheck(TimePoint current);

 private:
  TimePoint m_lastPingTime;  // 클라->서버 ping 도착 시간

 public:
  static uint64_t GetHash(const sockaddr_in& addrInfo) {
    // ipv4기준
    // ip는 32비트 << 16 shift + port 16비트
    // 해시키 생성
    return static_cast<uint64_t>(addrInfo.sin_addr.s_addr << 16 | addrInfo.sin_port);
    ;
  }
};
}  // namespace sh::UDP_Echo
