#pragma once
#include <atomic>
#include <memory>
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"

namespace sh::IO_Engine {
enum SESSION_STATE : BYTE {
  NON_ERR = 0x0,
  SEND_ERR = 0x1,
  RECV_ERR = 0x2,
  DISCONNECT_STATE = 0x3,
};
class ISendContext;
class IRecvContext;

class ISession
    : public Utility::IWorkerItem {
 public:
  ISession();

  ISession(SOCKET sock, const IO_TYPE ioType, RecvHandler recvHandler, HANDLE iocpHandle);

  virtual ~ISession();

  void DoSend(const void* data, const size_t len);

  virtual void Execute(Utility::ThWorkerJob* thWorkerItem, const DWORD ioByte, const uint64_t errorCode) override;

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
  std::unique_ptr<ISendContext> m_sendContext;
  std::unique_ptr<IRecvContext> m_recvContext;
  HANDLE m_iocpHandle;
  SOCKET m_sock;
  std::atomic<SESSION_STATE> m_state;
  std::atomic_bool m_isDisconnnected;
};
}  // namespace sh::IO_Engine
