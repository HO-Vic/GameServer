#include "pch.h"
#include <IO_Core/Acceptor/Acceptor.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
Acceptor::Acceptor()
    : m_iocpHandle(nullptr), m_listenSocket(NULL), m_acceptCompleteHandle(nullptr), m_acceptNo(1) {
}
Acceptor::Acceptor(HANDLE iocpHandle, SOCKET listenSocket, AcceptCompleteHandler acceptHandleFunc, const uint8_t acceptNo)
    : m_iocpHandle(iocpHandle), m_listenSocket(listenSocket), m_acceptCompleteHandle(std::move(acceptHandleFunc)), m_acceptNo(acceptNo) {
  if (nullptr == m_iocpHandle) {
#ifdef _DEBUG
    assert(nullptr != m_iocpHandle && "Invalid ioHandle");
#endif  // _DEBUG
  }
  if (m_acceptNo < 1) {
#ifdef _DEBUG
    assert(m_acceptNo >= 1 && "m_acceptNo less than 1");
#endif  // _DEBUG
  }
}

void Acceptor::Init(HANDLE iocpHandle, SOCKET listenSocket, AcceptCompleteHandler acceptHandleFunc, const uint8_t acceptNo) {
  m_iocpHandle = iocpHandle;
  m_listenSocket = listenSocket;
  m_acceptCompleteHandle = std::move(acceptHandleFunc);
  m_acceptNo = acceptNo;

  if (nullptr == m_iocpHandle) {
#ifdef _DEBUG
    assert(nullptr != m_iocpHandle && "Invalid IoHandle");
#endif  // _DEBUG
  }
  if (m_acceptNo < 1) {
#ifdef _DEBUG
    assert(m_acceptNo >= 1 && "m_acceptNo less than 1");
#endif  // _DEBUG
  }
}

void Acceptor::Init(HANDLE iocpHandle, AcceptCompleteHandler acceptHandleFunc, const uint8_t acceptNo) {
  m_iocpHandle = iocpHandle;
  m_acceptCompleteHandle = std::move(acceptHandleFunc);
  m_acceptNo = acceptNo;
}

void Acceptor::SetListenSocket(SOCKET listenSocket) {
  m_listenSocket = listenSocket;
}

void Acceptor::Start(uint16_t inetType, int socketType, int protocolType, bool isNoDelay) {
  for (uint8_t i = 0; i < m_acceptNo; ++i) {
    auto acceptEvent = std::make_shared<AcceptEvent>(m_listenSocket, m_acceptCompleteHandle, inetType, socketType, protocolType, isNoDelay);
    auto workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<Utility::IWorkerItem>(acceptEvent), Utility::WORKER_TYPE::ACCEPT);
    acceptEvent->Start(workerJob);
  }
}
}  // namespace sh::IO_Engine
