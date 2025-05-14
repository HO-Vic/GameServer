#include "pch.h"
#include "pch.h"
#include <Session/RecvContext/UDP_RecvContext.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <Session/UDP_IAgent.h>

namespace sh::IO_Engine {
bool UDP_RecvContext::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
  if (workerJob->GetType() != Utility::WORKER_TYPE::RECV) {
    return false;
  }
  auto sessionPtr = m_sessionPtr.lock();
  if (0 != errorCode || sessionPtr->GetState() == UDP_IAgent::STATE::INACTIVE) {  // 세션이 비활성화 상태라면
    sessionPtr->DestroyFromReceiver();
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return true;
  }

  RecvComplete(ioByte, sessionPtr);

  auto ioError = DoRecv(workerJob, sessionPtr);
  if (0 != ioError) {
    sessionPtr->StopReq();
    sessionPtr->DestroyFromReceiver();
    ThWorkerJobPool::GetInstance().Release(workerJob);
  } else if (sessionPtr->GetState() == UDP_IAgent::STATE::INACTIVE) {  // WSARecvFrom() 호출 이후에 세션이 종료됐다면
    // IOCP로 해당 객체 Error 들어와서 14번줄에서 처리
    CancelIoEx(reinterpret_cast<HANDLE>(sessionPtr->GetSocket()), workerJob);
  }
  return true;
}

void UDP_RecvContext::RecvComplete(uint32_t ioSize, std::shared_ptr<UDP_IAgent>& sessionPtr) {
  // UDP 패킷인데, 패킷 여러개가 뭉쳐서 올 수 있음(어플리케이션에서 WSABUF 여러개 묶는 경우), 보통은 1개씩만
  auto currentPosition = m_buffer;
  while (ioSize > 0) {
    auto rawPacket = reinterpret_cast<PacketHeader*>(currentPosition);
    if (ioSize < rawPacket->size) {
      break;
    }
    m_recvHandler(sessionPtr, rawPacket->size, currentPosition);
    ioSize -= rawPacket->size;
    currentPosition = currentPosition += rawPacket->size;
  };
}

int32_t UDP_RecvContext::DoRecv(Utility::ThWorkerJob* thWorkerJob, std::shared_ptr<UDP_IAgent>& sessionPtr) {
  DWORD recvByte = 0;
  DWORD flag = 0;
  auto ioError = WSARecvFrom(sessionPtr->GetSocket(), &m_wsaBuf, 1, &recvByte, &flag, nullptr, 0, thWorkerJob, nullptr);
  if (0 != ioError) {
    ioError = WSAGetLastError();
    if (ioError == ERROR_IO_PENDING) {
      ioError = 0;
    }
  }
  return ioError;
}
}  // namespace sh::IO_Engine
