#ifndef WIN32_NO_STATUS
#include <ntstatus.h>  //이거 먼저 안하면 + WIN32_NO_STATUS안하면 메크로 중복 정의 경고
#define WIN32_NO_STATUS
#endif
#include <pch.h>
#include <Buffer/SendBuffer.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>
#include <Session/UDP_IAgent.h>
#include <Session/UDP_ISession.h>

namespace sh::IO_Engine {
SendBuffer::SendBuffer(const BYTE* data, const uint32_t len)
    : m_size(len + sizeof(PacketHeader)) {
  PacketHeader packetSize{static_cast<uint16_t>(m_size)};
  packetSize.Serialize();
  memcpy_s(m_buffer, MAX_SEND_BUFFER_SIZE, &packetSize, sizeof(PacketHeader));
  memcpy_s(m_buffer + sizeof(PacketHeader), static_cast<int64_t>(MAX_SEND_BUFFER_SIZE - sizeof(PacketHeader)), data, len);
}

UDP_SingleSendBuffer::UDP_SingleSendBuffer(std::shared_ptr<UDP_IAgent>& agentPtr, std::shared_ptr<UDP_ISession>& sessionPtr, const BYTE* data, const uint32_t len)
    : SendBuffer(data, len), m_wsaBuf(len, const_cast<char*>(reinterpret_cast<const char*>(data))), m_agentPtr(agentPtr), m_sessionPtr(sessionPtr), m_retransmitCnt(0) {
}

bool UDP_SingleSendBuffer::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) {
  static constexpr uint32_t MAX_RETRASMIT = 5;  // 패킷 단위로 재전송 횟수 기록

  if (workerJob->GetType() != Utility::WORKER_TYPE::SEND) {  // 해당 객체는 Send Completion을 제외하고는 올 수 없음
#ifdef _DEBUG
    assert(workerJob->GetType() != Utility::WORKER_TYPE::SEND);
#endif  // _DEBUG
    return false;
  }
  ThWorkerJobPool::GetInstance().Release(workerJob);  // this 클래스는 refCnt=0이면 알아서 커스텀 딜리터로 감

  if (errorCode == STATUS_PORT_UNREACHABLE || errorCode == STATUS_CONNECTION_RESET) {  // 재전송 시도
    auto agentPtr = m_agentPtr.lock();
    auto sessionPtr = m_sessionPtr.lock();
    if (nullptr != agentPtr && nullptr != sessionPtr) {  // 둘 다 유효하면 재시도
      if (agentPtr->GetState() == UDP_IAgent::STATE::INACTIVE) {
        return true;
      }
      if (m_retransmitCnt == MAX_RETRASMIT) {
        sessionPtr->Close();
        return false;
      }
      m_retransmitCnt++;
      DoSend(agentPtr->GetSocket(), sessionPtr->GetAddrInfo());
    }
  }

  return true;
}

uint32_t UDP_SingleSendBuffer::DoSend(SOCKET sock, sockaddr_in toAddr) {
  DWORD sendByte = 0;
  DWORD flag = 0;
  auto selfPtr = shared_from_this();
  auto workJobPtr = ThWorkerJobPool::GetInstance().GetObjectPtr(selfPtr, Utility::WORKER_TYPE::SEND);
  auto ioError = WSASendTo(sock, &m_wsaBuf, 1, &sendByte, flag, reinterpret_cast<SOCKADDR*>(&toAddr), sizeof(toAddr), workJobPtr, nullptr);
  if (0 != ioError) {
    ioError = WSAGetLastError();
    if (WSA_IO_PENDING != ioError) {
      // 실패했다면 객체 되돌리기
      ThWorkerJobPool::GetInstance().Release(workJobPtr);
      return ioError;
    }
  }
  return 0;
}
}  // namespace sh::IO_Engine