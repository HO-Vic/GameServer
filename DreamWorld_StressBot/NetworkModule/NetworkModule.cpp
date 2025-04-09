#include "pch.h"
#include "NetworkModule.h"
#include <functional>
#include "../Session/SessionManager.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"

namespace Stress {
void NetworkModule::Init(const std::string& ipAddr, uint16_t port) {
  InitMsgDispatcher();
  m_ioCore.Init(4);
  m_connector.Init(m_ioCore.GetHandle(), ipAddr, port, AF_INET, SOCK_STREAM, IPPROTO_TCP, TIMEOUT_MS);
}

void NetworkModule::Start() {
  m_ioCore.Start();
  while (true) {
    m_connector.TryConnect(std::bind(&NetworkModule::OnConnect, this, std::placeholders::_1), std::bind(&NetworkModule::OnConnectFail, this));
  }
}

void NetworkModule::OnConnect(SOCKET sock) {
  using namespace std::placeholders;
  SessionManager::GetInstance().OnConnect(sock, sh::IO_Engine::IO_TYPE::TCP, m_ioCore.GetHandle(), std::bind(&NetworkModule::RecvHandle, this, _1, _2, _3));
}

void NetworkModule::OnConnectFail() {
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