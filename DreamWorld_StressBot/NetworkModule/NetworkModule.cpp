#include "pch.h"
#include "NetworkModule.h"
#include <functional>
#include <ws2def.h>
#include <WinSock2.h>
#include <winsock.h>
#include <Utility/Job/Job.h>
#include "../Session/SessionManager.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"
#include "../LogManager/LogManager.h"
#include "../Session/SessionBatchUpdaters.h"
#include "../Session/SessionBatchUpdater.h"
#include "../GlobalObjectPool/GlobalObjectPool.h"

namespace Stress {
void NetworkModule::Init(const std::string& ipAddr, uint16_t port) {
  InitMsgDispatcher();
  m_ioCore.Init(4);
  m_connector.Init(m_ioCore.GetHandle(), ipAddr, port, AF_INET, SOCK_STREAM, IPPROTO_TCP, TIMEOUT_MS);
}

void NetworkModule::Start() {
  m_ioCore.Start();
  WRITE_LOG(logLevel::info, "{}({}) > Startr Network Module!!", __FUNCTION__, __LINE__);
  while (true) {
    m_connector.TryConnect(std::bind(&NetworkModule::OnConnect, this, std::placeholders::_1), std::bind(&NetworkModule::OnConnectFail, this, std::placeholders::_1));
  }
}

void NetworkModule::OnConnect(SOCKET sock) {
  using namespace std::placeholders;
  auto sessionPtr = SessionManager::GetInstance().OnConnect(sock, sh::IO_Engine::IO_TYPE::TCP, m_ioCore.GetHandle(), std::bind(&NetworkModule::RecvHandle, this, _1, _2, _3));
  sessionPtr->Init();
  sessionPtr->StartRecv();

  auto batchUpdater = SessionBatchUpdaters::GetInstance().GetBatchUpdater(sessionPtr->GetUniqueNo());
  auto jobPtr = GlobalObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([batchUpdater, sessionPtr]() {
    batchUpdater->InsertSession(sessionPtr);
  });
  batchUpdater->InsertJob(std::move(jobPtr));
}

void NetworkModule::OnConnectFail(int errorCode) {
  WRITE_LOG(logLevel::debug, "{}({}) > Connect Fail!! [WSAErrorCode:{}]", __FUNCTION__, __LINE__, errorCode);
}

void NetworkModule::RecvHandle(sh::IO_Engine::ISessionPtr sessionPtr, size_t, BYTE* bufferPosition) {
  auto packetHeader = reinterpret_cast<DreamWorld::PacketHeader*>(bufferPosition);
  MsgHandler handler = nullptr;
  if (!m_msgDispatcher.GetHandler(packetHeader->type, handler)) {  // 없으면 에러 처리하지 않음, 스트레스 테스트에서 처리하지 않는게 많음
    return;
  }
  handler(sessionPtr, bufferPosition);
}
}  // namespace Stress