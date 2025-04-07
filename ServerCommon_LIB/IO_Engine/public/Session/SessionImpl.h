#pragma once
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"
#include "../Session/SendContext/SendContext.h"
#include "../Session/RecvContext/RecvContext.h"

/*
IO 에러 발생 상황
1. Send 완료 처리
  a. Disconnect 상태로 변경
  case 1. Recv 완료 처리중
   - Disconnect Stete보고서 Recv 시도 안함
   Disconnect 처리 완료.
   - 처리 못했다면, 다음 완료 처리에서 하면 됨
  case 2. Recv 시도
   - 시도 전에 Disconnect sate보고서 안하면 좋음
   - 했다면 Recv 완료 처리에서 하면 됨
   Disconnect 처리 완료.
   //그렇다면, Send, Recv 각 각 Disconnect 잘했는지만 확인
   // 후에, 둘 다 처리 됐다면, 상위 레이어 처리

2. Send 발생 발생
  case 1. Recv 완료 대기

3. Recv 완료 처리

4. Recv 시도 발생

나머지 케이스도 비슷할거라 생각
*/
namespace sh::IO_Engine {
enum SESSION_STATE : BYTE {
  NON_ERR = 0x0,
  SEND_ERR = 0x1,
  RECV_ERR = 0x2,
  DISCONNECT_STATE = 0x3,
};

class SessionImpl {
 public:
  SessionImpl(SOCKET sock, const IO_TYPE ioType, RecvHandler&& recvHandler, HANDLE iocpHandle);

  void SendComplete(Utility::ThWorkerJob* thWorkerJob, const DWORD ioByte);

  void DoSend(Utility::WokerPtr session, const BYTE* data, const size_t len);

  void RecvComplete(Utility::ThWorkerJob* thWorkerJob, DWORD ioByte);

  void StartRecv(Utility::WokerPtr session);

  void RaisedIoError(Utility::ThWorkerJob* thWorkerJob);

 private:
  SendContext m_sendContext;
  RecvContext m_recvContext;
  HANDLE m_iocpHandle;
  std::atomic<SESSION_STATE> m_state;
};
}  // namespace sh::IO_Engine