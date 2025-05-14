#pragma once
#include <atomic>
#include <memory>
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"
#include "SendContext/TCP_SendContext.h"
#include "RecvContext/TCP_RecvContext.h"

namespace sh::IO_Engine {
enum TCP_Session_STATE : BYTE {
  NON_ERR = 0x0,
  SEND_ERR = 0x1,
  RECV_ERR = 0x2,
  DISCONNECT_STATE = 0x3,
};

class TCP_ISession
    : public Utility::IWorkerItem {
 public:
  TCP_ISession();

  TCP_ISession(SOCKET sock, [[maybe_unused]] const IO_TYPE ioType, TCP_RecvHandler recvHandler, HANDLE iocpHandle);

  virtual ~TCP_ISession();

  void DoSend(const void* data, const size_t len);

  virtual bool Execute(Utility::ThWorkerJob* thWorkerItem, const DWORD ioByte, const uint64_t errorCode) override;

  void StartRecv();

  bool IsDisconnected() const;

 private:
  void RaiseIOError();

  void RaiseIOError(sh::Utility::ThWorkerJob* thWorker);

 protected:
  // 위에 레이어에서 상속받아서 Disconnect 상황에서 해야하는 일 정의
  virtual void OnDisconnect() = 0;

 private:
  void Disconnect();

 private:
  TCP_SendContext m_sendContext;
  TCP_RecvContext m_recvContext;
  HANDLE m_iocpHandle;
  SOCKET m_sock;
  std::atomic<TCP_Session_STATE> m_state;
  std::atomic_bool m_isDisconnnected;
};
}  // namespace sh::IO_Engine
