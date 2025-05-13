#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <cstdint>
#include <Windows.h>
#include "Utility/Thread/IWorkerItem.h"

namespace sh::IO_Engine {
// UDP는 독립 데이터 그램이 오기 때문에, Receiver를 여러 개 소유
// Send는 데이터 그램 단위로 보내기때문에, Session에서 따로 관리xx
class UDP_Receiver;
class UDP_ISession
    : public Utility::IWorkerItem {
  enum class STATE : BYTE {
    ACTIVE,
    INACTIVE
  };

 public:
  UDP_ISession() = default;

  UDP_ISession(SOCKET sock, uint32_t receiverNo, uint16_t port = 9000);

  virtual ~UDP_ISession() = default;

  // 모든 UDP리시버가 종료 될 수 있게
  // Destroy
  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

  virtual void OnDestroy();

  // CancelIOEx(m_socket); 호출하여 모든 IO종료
  void Destroy();

  // 외부에서 주소를 주면 해당 멤버 socket Send
  bool Send(uint32_t toIp, uint16_t toPort, const BYTE* Data, const size_t len);

  bool Send(SOCKADDR& toAddr, const BYTE* Data, const size_t len);

  void StartRecv();

 private:
  std::vector<std::shared_ptr<UDP_Receiver>> m_receiver;
  SOCKET m_sock;
  std::atomic<uint32_t> m_activeReceiverCnt = 0;  // Receiver들은 활동할 때,++, 아니면 --
  uint16_t m_port;
  std::atomic<STATE> m_state = STATE::ACTIVE;
};
}  // namespace sh::IO_Engine
