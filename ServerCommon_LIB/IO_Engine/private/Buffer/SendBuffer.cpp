#include <pch.h>
#include <Buffer/SendBuffer.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
SendBuffer::SendBuffer(const BYTE* data, const uint32_t len)
    : m_size(len) {
  memcpy_s(m_buffer, MAX_SEND_BUFFER_SIZE, data, m_size);
}

UDP_SingleSendBuffer::UDP_SingleSendBuffer(const BYTE* data, const uint32_t len)
    : SendBuffer(data, len), m_wsaBuf(len, const_cast<char*>(reinterpret_cast<const char*>(data))) {
}

bool UDP_SingleSendBuffer::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
  bool retVal = true;
  if (workerJob->GetType() != Utility::WORKER_TYPE::SEND) {  // 해당 객체는 Send Completion을 제외하고는 올 수 없음
#ifdef _DEBUG
    assert(workerJob->GetType() != Utility::WORKER_TYPE::SEND);
#endif  // _DEBUG
    retVal = false;
  }
  ThWorkerJobPool::GetInstance().Release(workerJob);  // this 클래스는 refCnt=0이면 알아서 커스텀 딜리터로 감
  return retVal;
}

void UDP_SingleSendBuffer::DoSend(SOCKET sock, const SOCKADDR& toAddr) {
  DWORD sendByte = 0;
  DWORD flag = 0;
  auto selfPtr = shared_from_this();
  auto workJobPtr = ThWorkerJobPool::GetInstance().GetObjectPtr(selfPtr, Utility::WORKER_TYPE::SEND);
  auto ioError = WSASendTo(sock, &m_wsaBuf, 1, &sendByte, flag, &toAddr, sizeof(toAddr), workJobPtr, nullptr);
  if (0 != ioError) {
    ioError = WSAGetLastError();
    if (WSA_IO_PENDING == ioError) {
      return;
    }
  }
  // 실패했다면 객체 되돌리기
  ThWorkerJobPool::GetInstance().Release(workJobPtr);
}
}  // namespace sh::IO_Engine