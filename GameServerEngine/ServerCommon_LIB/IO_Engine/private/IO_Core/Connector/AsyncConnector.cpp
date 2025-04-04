#include "pch.h"
#include <IO_Core/Connector/AsyncConnector.h>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
AsyncConnector::IntenalTimer::IntenalTimer()
    : m_timeOutThread(nullptr) {
}

AsyncConnector::IntenalTimer::~IntenalTimer() {
  if (nullptr != m_timeOutThread) {
    m_timeOutThread->request_stop();
    m_timeOutThread->join();
  }
}

void AsyncConnector::IntenalTimer::Start() {
  m_timeOutThread = std::make_unique<std::jthread>([this](std::stop_token stopToken) {
    TimeOutFunc(stopToken);
  });
}

void AsyncConnector::IntenalTimer::TimeOutFunc(std::stop_token stopToken) {
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

AsyncConnector::AsyncConnector(HANDLE ioHandle, const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType, const MS timeOutThreshold)
    : ConnectorBase(ipAddr, port, inetType, socketType, protocolType), m_ioHandle(ioHandle), m_timeOutThreshold(timeOutThreshold), m_intenalTimer(nullptr) {
  // 타임 아웃이 없다면 ,타임 아웃 이벤트를 해줄 타이머를 만들지 않는다
  if (m_timeOutThreshold != MS(0)) {
    m_intenalTimer = std::make_unique<IntenalTimer>();
    m_intenalTimer->Start();
  }
  // ConnectEx 함수 포인터 획득을 위한 소켓으로 획득했다면 close
  SOCKET immediateSocket = WSASocketW(inetType, socketType, protocolType, NULL, NULL, WSA_FLAG_OVERLAPPED);
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

bool AsyncConnector::TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) {
  auto connectEvent = std::make_shared<AsyncConnectEvent>(std::move(successHandle), std::move(failHandle), ConnectEx, m_connectAddr);
  auto thWorkerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(connectEvent, Utility::WORKER_TYPE::CONNECT);
  return connectEvent->TryConnect(thWorkerJob, m_inetType, m_socketType, m_protocolType);
}

AsyncConnectEvent::AsyncConnectEvent(ConnectCompleteHandler&& successHandle, ConnectFailHandler&& failHandle, LPFN_CONNECTEX connectEx, const SOCKADDR_IN connectAddr)
    : m_successHandle(successHandle), m_failHandle(failHandle), m_socket(NULL), ConnectEx(connectEx), m_connectAddr(connectAddr), m_connectingState(STATE::TRY_CONNECT) {
}

bool AsyncConnectEvent::TryConnect(Utility::ThWorkerJob* thWorkerJob, uint16_t inetType, int socketType, int protocolType) {
  if (NULL == m_socket) {
    m_socket = WSASocketW(inetType, socketType, protocolType, NULL, NULL, WSA_FLAG_OVERLAPPED);
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
    m_connectingState = STATE::CONNECTED;  // 성공했다면, ICCP에서 완료 처리가 오지 않음
    return true;
  }

  auto errorCode = WSAGetLastError();
  if (WSA_IO_PENDING != errorCode) {
    m_failHandle(errorCode);
    m_connectingState = STATE::TIMEOUT;
    return false;
  }

  // IO Pending
  return true;
}

void AsyncConnectEvent::Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) {
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

std::function<void()> AsyncConnectEvent::GetTimeOutFunc() {
  return std::bind(&AsyncConnectEvent::TimeOut, std::static_pointer_cast<AsyncConnectEvent>(shared_from_this()));
}

void AsyncConnectEvent::TimeOut() {
  auto expectedState = STATE::TRY_CONNECT;
  if (!m_connectingState.compare_exchange_strong(expectedState, STATE::TIMEOUT)) {  // 타임 아웃 걸었지만, 이전에 성공했다면 수행하지 않음
    return;
  }
  if (m_socket != NULL) {
    closesocket(m_socket);
    m_socket = NULL;
  }
  m_failHandle(WSAGetLastError());
}
}  // namespace sh::IO_Engine