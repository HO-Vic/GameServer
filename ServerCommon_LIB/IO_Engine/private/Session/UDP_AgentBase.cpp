#include "pch.h"
#include <Session/UDP_AgentBase.h>
#include <memory>
#include <Windows.h>
#include <ioapiset.h>
#include <Session/RecvContext/UDP_RecvContext.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
UDP_AgentBase::UDP_AgentBase(SOCKET sock, uint32_t receiverNo, uint16_t port)
    : m_socket(sock), m_activeReceiverCnt(receiverNo), m_port(port) {
#ifdef _DEBUG
  sockaddr_in addrInfo{};
  int addrLen = sizeof(sockaddr_in);
  auto sockResult = getsockname(m_socket, reinterpret_cast<sockaddr*>(&addrInfo), &addrLen);
  if (sockResult != 0) {
    assert(sockResult != 0 && "Invalid Socket");  // 바인드 안된 경우
  } else {
    // ANY_ADDR
    //  if (ntohl(addrInfo.sin_addr.s_addr) == 0) {
    //    assert(sockResult != 0 && "Invalid Socket");  // 바인드 안된 경우
    //  }
  }
#endif
}

UDP_AgentBase::~UDP_AgentBase() {
  //  ReleaseSocket(m_socket); //pure virtual func
  // ~derived() -> ~parent() 에서 derived의 vtable ptr -> parent vtable ptr로 변경
  // pure virtual만 있고 구현체?가 없으니 호출을 할 수가 없음
}

void UDP_AgentBase::StopReq() {
  // 해당 socket에 대해서 모든 이벤트 종료
  m_state = STATE::INACTIVE;
  CancelIoEx(reinterpret_cast<HANDLE>(m_socket), nullptr);
}

bool UDP_AgentBase::StartRecv(UDP_RecvHandler recvHandle) {
  auto thisPtr = shared_from_this();
  auto cnt = m_activeReceiverCnt.load();
  for (unsigned int i = 0; i < cnt; ++i) {
    m_receiver.push_back(std::make_shared<UDP_RecvContext>(thisPtr, recvHandle));
    auto thWorkerPtr = ThWorkerJobPool::GetInstance().GetObjectPtr(m_receiver.back(), Utility::WORKER_TYPE::RECV);
    if (0 != m_receiver.back()->DoRecv(thWorkerPtr, thisPtr)) {
      // 에러
      if (i > 0) {
        StopReq();
      } else {
        OnDestroy();  // 아직 요청이 들어가지 않았기때문에 Destroy 호출
      }
      ThWorkerJobPool::GetInstance().Release(thWorkerPtr);
      return false;
    }
  }
  return true;
}

void UDP_AgentBase::DestroyFromReceiver() {
  m_activeReceiverCnt--;
  if (0 == m_activeReceiverCnt) {  // 더 이상 파괴할 recv 객체가 없다면 destroy
    OnDestroy();
  }
}
}  // namespace sh::IO_Engine
