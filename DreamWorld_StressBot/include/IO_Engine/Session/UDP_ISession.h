#pragma once

namespace sh::IO_Engine {
// UDP는 1:n 통신이 가능하기 때문에, Session당 소켓 x
// UDP_IAgent의 소켓을 사용하여 Send
class UDP_ISession {
 public:
  UDP_ISession() = default;

  UDP_ISession(const SOCKADDR& toAddr)
      : m_toAddr(toAddr) {
  }

  virtual ~UDP_ISession() = default;

  void DoSend(SOCKET sock, const BYTE* data, uint32_t len) const;

 private:
  SOCKADDR m_toAddr;
};

class UDP_IBatchSession {
};
}  // namespace sh::IO_Engine
