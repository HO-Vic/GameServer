#include "pch.h"
#include <memory>
#include <Windows.h>
#include <ioapiset.h>
#include <Session/UDP_IAgent.h>
#include <Session/RecvContext/UDP_RecvContext.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
UDP_IAgent::UDP_IAgent(SOCKET sock, uint32_t receiverNo, uint16_t port)
    : m_socket(sock), m_activeReceiverCnt(receiverNo), m_port(port) {
#ifdef _DEBUG
  sockaddr_in addrInfo{};
  int addrLen = sizeof(sockaddr_in);
  auto sockResult = getsockname(m_socket, reinterpret_cast<sockaddr*>(&addrInfo), &addrLen);
  if (sockResult != 0) {
    assert(sockResult != 0 && "Invalid Socket");  // 바인드 안된 경우
  } else {
    if (ntohl(addrInfo.sin_addr.s_addr) == 0) {
      assert(sockResult != 0 && "Invalid Socket");  // 바인드 안된 경우
    }
  }
#endif
}

UDP_IAgent::~UDP_IAgent() {
  ReleaseSocket(m_socket);
}

void UDP_IAgent::StopReq() {
  // 해당 socket에 대해서 모든 이벤트 종료
  m_state = STATE::INACTIVE;
  CancelIoEx(reinterpret_cast<HANDLE>(m_socket), nullptr);
}

bool UDP_IAgent::StartRecv() {
  auto thisPtr = shared_from_this();
  auto cnt = m_activeReceiverCnt.load();
  for (unsigned int i = 0; i < cnt; ++i) {
    m_receiver.push_back(std::make_shared<UDP_RecvContext>());
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

void UDP_IAgent::DestroyFromReceiver() {
  m_activeReceiverCnt--;
  if (0 == m_activeReceiverCnt) {  // 더 이상 파괴할 recv 객체가 없다면 destroy
    OnDestroy();
  }
}
}  // namespace sh::IO_Engine
