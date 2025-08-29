#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include "pch.h"
#include <memory>
#include <Session/RecvContext/UDP_RecvContext.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <Session/UDP_IAgent.h>

namespace sh::IO_Engine {
bool UDP_RecvContext::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  if (workerJob->GetType() != Utility::WORKER_TYPE::RECV) {
    return false;
  }

  auto agentPtr = m_agentPtr.lock();
  if (nullptr == agentPtr) {
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return false;
  }

  if (STATUS_SUCCESS != errorCode) {
    switch (errorCode) {
      case STATUS_CANCELLED: {            // CancelIoEx();//io작업 취소
        agentPtr->DestroyFromReceiver();  // 현재 리시버를 종료
        ThWorkerJobPool::GetInstance().Release(workerJob);
        return true;
        break;
      }

      case STATUS_BUFFER_OVERFLOW:
      case STATUS_INSUFFICIENT_RESOURCES: {  // 재시도, 밑에 else-if안타고 DoRecv할거기 때문에 따른 작업은 xx
        break;
      }

      case STATUS_ACCESS_VIOLATION:
      case STATUS_INVALID_HANDLE: {  // 소켓, 시스템 에러
        agentPtr->DestroyFromReceiver();
        ThWorkerJobPool::GetInstance().Release(workerJob);
        return false;
        break;
      }

      default:
        break;
    }

    return false;  //
  } else {
    if (agentPtr->GetState() == UDP_IAgent::STATE::INACTIVE) {  // 세션이 비활성화 상태라면
      agentPtr->DestroyFromReceiver();                          // 현재 리시버 종료
      ThWorkerJobPool::GetInstance().Release(workerJob);
      return true;
    }

    RecvComplete(ioByte, agentPtr);  // 에러가 없는 경우만 수행
  }

  auto ioError = DoRecv(workerJob, agentPtr);

  if (0 != ioError) {
#ifdef _DEBUG
    /*
    WSAENOTSOCK(10038) → 잘못된 소켓 핸들

    WSAESHUTDOWN(10058) → 이미 shutdown 된 소켓

    WSANOTINITIALISED(10093) → WSAStartup 문제(프로세스 전역 문제)

    WSAENETDOWN(10050) → 네트워크 서브시스템 다운

    WSAEFAULT (10014) → 버퍼 잘못 지정 (코드 버그)

    WSAEINVAL (10022) → 파라미터 오류 (옵션 세팅 문제)

    WSAEOPNOTSUPP (10045) → 소켓 타입이 잘못됨
    */
    assert(false && "ioError");
#endif  // _DEBUG

    agentPtr->StopReq();
    agentPtr->DestroyFromReceiver();
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return false;
  }

  // else if (agentPtr->GetState() == UDP_IAgent::STATE::INACTIVE) {  // WSARecvFrom() 호출 이후에 세션이 종료됐다면
  //   // IOCP로 해당 객체 Error 들어와서 14번줄에서 처리
  //   CancelIoEx(reinterpret_cast<HANDLE>(agentPtr->GetSocket()), workerJob);
  // }
  return true;
}

void UDP_RecvContext::RecvComplete(uint32_t ioSize, std::shared_ptr<UDP_IAgent>& agentPtr) {
  // UDP 패킷인데, 패킷 여러개가 뭉쳐서 올 수 있음(어플리케이션에서 WSABUF 여러개 묶는 경우), 보통은 1개씩만
  auto currentPosition = m_buffer;
  while (ioSize > 0) {
    auto rawPacket = reinterpret_cast<PacketHeader*>(currentPosition);
    if (ioSize < rawPacket->size) {
      break;
    }
    m_recvHandler(agentPtr, rawPacket->size, currentPosition);
    ioSize -= rawPacket->size;
    currentPosition = currentPosition += rawPacket->size;
  };
}

int32_t UDP_RecvContext::DoRecv(Utility::ThWorkerJob* workerJob, std::shared_ptr<UDP_IAgent>& agentPtr) {
  DWORD recvByte = 0;
  DWORD flag = 0;
  auto ioError = WSARecvFrom(agentPtr->GetSocket(), &m_wsaBuf, 1, &recvByte, &flag, nullptr, 0, workerJob, nullptr);
  if (0 != ioError) {
    ioError = WSAGetLastError();
    if (ioError == ERROR_IO_PENDING) {
      ioError = 0;
    }
  }
  return ioError;
}
}  // namespace sh::IO_Engine
