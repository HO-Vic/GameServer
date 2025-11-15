#include "pch.h"
#include <IO_Core/Acceptor/Acceptor.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Pool/ObjectPool.h>
#include <IO_Core/Acceptor/AcceptEvent.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
Acceptor::Acceptor()
    : m_acceptCompleteHandle(nullptr), m_acceptNo(1), m_registToIocp(true) {
}

Acceptor::Acceptor(AcceptCompleteHandler acceptHandleFunc, const bool registToIocp /*= true*/, const uint8_t acceptNo /*= 1*/)
    : m_acceptCompleteHandle(acceptHandleFunc), m_acceptNo(acceptNo) {
#ifdef _DEBUG
  assert(m_acceptNo >= 1 && "m_acceptNo less than 1");
#endif  // _DEBUG
}

void Acceptor::Init(AcceptCompleteHandler acceptHandleFunc, const bool registToIocp /* = true*/, const uint8_t acceptNo /*= 1*/) {
  m_acceptCompleteHandle = std::move(acceptHandleFunc);
  m_acceptNo = acceptNo;

#ifdef _DEBUG
  assert(m_acceptNo >= 1 && "m_acceptNo less than 1");
#endif  // _DEBUG
}

void Acceptor::Start(SOCKET listenSocket, HANDLE iocpHandle, uint16_t inetType, int socketType, int protocolType, bool isNoDelay) {
  for (uint8_t i = 0; i < m_acceptNo; ++i) {
    auto acceptEvent = std::make_shared<AcceptEvent>(listenSocket, iocpHandle, m_acceptCompleteHandle, inetType, socketType, protocolType, isNoDelay, m_registToIocp);
    auto workerJob = ThWorkerJobPool::GetInstance().GetObjectPtr(std::static_pointer_cast<Utility::IWorkerItem>(acceptEvent), Utility::WORKER_TYPE::ACCEPT);
    acceptEvent->Start(workerJob);
  }
}
}  // namespace sh::IO_Engine
