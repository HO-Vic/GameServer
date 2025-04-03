#include "pch.h"
#include <IO_Core/Connector/Connector.h>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
Connector::IntenalTimer::IntenalTimer()
    : m_timeOutThread(nullptr) {
}

Connector::IntenalTimer::~IntenalTimer() {
  if (nullptr != m_timeOutThread) {
    m_timeOutThread->request_stop();
    m_timeOutThread->join();
  }
}

void Connector::IntenalTimer::Start() {
  m_timeOutThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
    TimeOutFunc(stopToken);
  });
}

void Connector::IntenalTimer::TimeOutFunc(std::stop_token stopToken) {
  while (true) {
    if (stopToken.stop_requested()) {
      return;
    }
    if (!m_timerJob.empty()) {
      auto job = m_timerJob.top().get();
      if (job->IsReady()) {
        job->Execute();
        m_timerJob.pop();
        continue;
      }
    }
    std::this_thread::yield();
  }
}

Connector::Connector(HANDLE ioHandle, const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType, const MS timeOutThreshold)
    : m_ioHandle(ioHandle), m_inetType(inetType), m_socketType(socketType), m_protocolType(protocolType), m_timeOutThreshold(timeOutThreshold), m_intenalTimer(nullptr) {
  ZeroMemory(&m_connectAddr, sizeof(SOCKADDR_IN));
  m_connectAddr.sin_family = m_inetType;
  m_connectAddr.sin_port = htons(port);
  inet_pton(m_inetType, ipAddr.data(), &m_connectAddr.sin_addr.s_addr);

  // 타임 아웃이 없다면 ,타임 아웃 이벤트를 해줄 타이머를 만들지 않는다
  if (m_timeOutThreshold != MS(0)) {
    m_intenalTimer = std::make_unique<IntenalTimer>();
    m_intenalTimer->Start();
  }
  // ConnectEx 함수 포인터 획득을 위한 소켓으로 획득했다면 close
  SOCKET immediateSocket = WSASocketA(inetType, socketType, protocolType, NULL, NULL, WSA_FLAG_OVERLAPPED);
  GUID guid = WSAID_CONNECTEX;
  DWORD bytesReturned = 0;
  auto result = WSAIoctl(immediateSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &ConnectEx, sizeof(ConnectEx), &bytesReturned, nullptr, nullptr);
  closesocket(immediateSocket);
  if (result != 0) {
    char errorString[1024] = {0};
    sprintf_s(errorString, "%s(%d) > Failed to Get ConnectEx Functor [errorCode: %d]", __FUNCTION__, __LINE__, WSAGetLastError());
    assert(false && errorString);
  }
}

bool Connector::TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) {
  auto connectEvent = std::make_shared<ConnectEvent>(std::move(successHandle), std::move(failHandle), ConnectEx);
  auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(connectEvent, Utility::WORKER_TYPE::CONNECT);
  return connectEvent->TryConnect(thWorkerJob, m_inetType, m_socketType, m_protocolType);
}

ConnectEvent::ConnectEvent(ConnectCompleteHandler&& successHandle, ConnectFailHandler&& failHandle, LPFN_CONNECTEX connectEx, const SOCKADDR_IN connectAddr)
    : m_successHandle(successHandle), m_failHandle(failHandle), m_socket(NULL), ConnectEx(connectEx), m_connectAddr(connectAddr), m_connectingState(STATE::TRY_CONNECT) {
}

bool ConnectEvent::TryConnect(Utility::ThWorkerJob* thWorkerJob, uint16_t inetType, int socketType, int protocolType) {
  if (NULL == m_socket) {
    m_socket = WSASocketA(inetType, socketType, protocolType, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (m_socket == INVALID_SOCKET) {
      return false;
    }
  } else {
    // Retry 소켓인데, valid 체크해야 됨
  }

  static constexpr size_t infoSize = sizeof(SOCKADDR_IN);
  DWORD sentByte = 0;
  bool result = ConnectEx(m_socket, reinterpret_cast<sockaddr*>(&m_connectAddr), static_cast<int>(infoSize), nullptr, 0, &sentByte, thWorkerJob);
  if (result) {
    m_successHandle(m_socket);
    m_socket = NULL;
    m_connectingState = STATE::CONNECTED;  // 성공했다면, ICCP 완료 처리 되지 않음
    return true;
  }

  auto errorCode = WSAGetLastError();
  if (WSA_IO_PENDING != errorCode) {
    m_failHandle(errorCode);
    return false;
  }

  // IO Pending
  return true;
}

void ConnectEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
  if (0 != errorCode) {
    STATE tryState = STATE::TRY_CONNECT;
    if (m_connectingState.compare_exchange_strong(tryState, STATE::TIMEOUT)) {  // 타임 아웃 상태로 변경하는데, 성공했다면
      m_failHandle(WSAGetLastError());
    }
    ThWorkerJobPool::GetInstance().Release(workerJob);
    return;
  }
  STATE tryState = STATE::TRY_CONNECT;
  if (m_connectingState.compare_exchange_strong(tryState, STATE::CONNECTED)) {  // 못바꾸면, timeOut이 먼저 불림
    m_successHandle(m_socket);
    m_socket = NULL;
  }
  ThWorkerJobPool::GetInstance().Release(workerJob);
}

std::function<void()> ConnectEvent::GetTimeOutFunc() {
  return std::function<void()>();
}
}  // namespace sh::IO_Engine