#pragma once
#include <atomic>
#include <memory>
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"
#include "SendContext/TCP_SendContext.h"
#include "RecvContext/TCP_RecvContext.h"

namespace sh::IO_Engine {
class SendBufferBase;
class AcceptEvent;
enum TCP_Session_STATE : BYTE {
  NON_ERR = 0x0,
  SEND_ERR = 0x1,
  RECV_ERR = 0x2,
  DISCONNECT_STATE = 0x3,
};

class TCP_SessionBase
    : public Utility::IWorkerItem {
 public:
  TCP_SessionBase() = default;

  TCP_SessionBase(SOCKET sock, [[maybe_unused]] const IO_TYPE ioType, TCP_RecvHandler recvHandler, HANDLE iocpHandle, std::shared_ptr<AcceptEvent>&& acceptEvent = nullptr);

  virtual ~TCP_SessionBase();

  void DefferedSet(SOCKET sock, TCP_RecvHandler recvHandler, HANDLE iocpHandle);

  /*
   개선?
    1. sendBuffer를 어플리케이션에서 할당
    2. 라이브러리로 sendBuffer를 보냄
    3. sendBuffer 보내기
     => protobuf처럼 직렬화를 어플리케이션에서 하고, DoSend(T)하면 복사 2번이니
        ㄴ> 차라리 sendBuffer를 어플리케이션 레이어에서 복사, 그 버퍼를 라이브러리로
  DoSend(T)
    1. data ptr을 라이브러리로
    2. 라이브러리에서 sendBuffer에 데이터 복사
  */

  // 외부에서 ptr에 데이터 정보 다 넣는 경우
  // 외부에서 msg.Serialize(&sendBuffer);해야하는 경우에, 미리 할당하고 sendBuffer로 내릴 수 있게
  void DoSend(std::shared_ptr<SendBufferBase>&& sendBuffer);

  template <class T>
  void DoSend(const T* data) {  // dataPtr을 라이브러리 내부에서 복사하여 Send하는 경우 사용
    InternalSend(reinterpret_cast<const BYTE*>(data), sizeof(T));
  }

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

  void StartRecv();

  bool IsDisconnected() const;

 private:
  void RaiseIOError();

  void RaiseIOError(sh::Utility::ThWorkerJob* thWorker);

 private:
  void InternalSend(const BYTE* data, const uint32_t len);

 protected:
  // 위에 레이어에서 상속받아서 Disconnect 상황에서 해야하는 일 정의
  virtual void OnDisconnect() = 0;

 protected:
  HANDLE m_iocpHandle = NULL;

 private:
  SOCKET m_sock = NULL;
  TCP_SendContext m_sendContext{};
  TCP_RecvContext m_recvContext{};
  std::shared_ptr<AcceptEvent> m_acceptEvent = nullptr;
  std::atomic<TCP_Session_STATE> m_state = NON_ERR;
  std::atomic_bool m_isDisconnnected = false;
};
}  // namespace sh::IO_Engine
