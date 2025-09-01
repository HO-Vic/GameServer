#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <cstdint>
#include "../CommonDefine.h"

namespace sh::IO_Engine {
class UDP_RecvContext;
class UDP_IAgent
    : public std::enable_shared_from_this<UDP_IAgent> {  // 세션보다는 에이전트가 좀 더 어울림
 public:
  enum class STATE : BYTE {
    ACTIVE,
    INACTIVE
  };

 public:
  UDP_IAgent() = default;

  // 소켓이 iocp 핸들에 등록되어 있어야 함
  UDP_IAgent(SOCKET sock, uint32_t receiverNo, [[maybe_unused]] uint16_t port = 9000);

  virtual ~UDP_IAgent();

  // 외부로 받은 소켓을 어떻게 정리할지
  virtual void ReleaseSocket(SOCKET socket) = 0;

  // 모든 UDP리시버가 종료됐을 때, 호출
  virtual void OnDestroy() = 0;

  void StopReq();

  bool StartRecv(UDP_RecvHandler recvHandle);

  void DestroyFromReceiver();

  STATE GetState() const {
    return m_state;
  }

  // 소멸자에서 소켓을 정리하기 때문에, ref갖고 있으면 socket이 유효하고, 아니라면 비유효
  SOCKET GetSocket() const {
    return m_socket;
  }

 private:
  SOCKET m_socket = INVALID_SOCKET;
  std::vector<std::shared_ptr<UDP_RecvContext>> m_receiver;
  std::atomic<uint32_t> m_activeReceiverCnt = 0;  // 모든 Receiver갯수
  uint16_t m_port = 0;
  std::atomic<STATE> m_state = STATE::ACTIVE;
};
}  // namespace sh::IO_Engine
