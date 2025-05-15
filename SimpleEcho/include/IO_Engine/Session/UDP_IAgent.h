#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <cstdint>
#include <Windows.h>

namespace sh::IO_Engine {
class UDP_RecvContext;
class UDP_IAgent {  // 세션보다는 에이전트가 좀 더 어울림
 public:
  enum class STATE : BYTE {
    ACTIVE,
    INACTIVE
  };

 public:
  UDP_IAgent() = default;

  UDP_IAgent(SOCKET sock, uint32_t receiverNo, uint16_t port = 9000);

  virtual ~UDP_IAgent();

  // 외부로 받은 소켓을 어떻게 정리할지
  virtual void ReleaseSocket(SOCKET socket) = 0;

  // 모든 UDP리시버가 종료됐을 때, 호출
  void Destroy();

  void StopReq();

  void StartRecv();

  void DestroyFromReceiver();

  STATE GetState() const {
    return m_state;
  }

  // 소멸자에서 소켓을 정리하기 때문에, ref갖고 있으면 socket이 유효하고, 아니라면 비유효
  SOCKET GetSocket() const {
    return m_socket;
  }

 private:
  SOCKET m_socket;
  std::vector<std::shared_ptr<UDP_RecvContext>> m_receiver;
  std::atomic<uint32_t> m_activeReceiverCnt = 0;  // 모든 Receiver갯수
  uint16_t m_port;
  std::atomic<STATE> m_state = STATE::ACTIVE;
};
}  // namespace sh::IO_Engine
