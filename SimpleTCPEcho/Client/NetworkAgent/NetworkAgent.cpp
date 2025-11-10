#include "pch.h"
#include "NetworkAgent.h"
#include "../LogMgr/LogManager.h"
#include "../Session/Session.h"
#include <IO_Engine/Session/TCP_ISession.h>

namespace SimpleTCP {
NetworkAgent::NetworkAgent(const uint8_t ioThreadNo, const std::string& ipAddr, const uint16_t portAddr)
    : m_core(ioThreadNo), m_connector(ipAddr, portAddr), m_connSession(nullptr), m_isConnSuccess(false) {
}

void NetworkAgent::Init() {
}

std::shared_ptr<Session> NetworkAgent::Connect(const ms timeOut) {
  using namespace std::chrono;
  m_connSession = std::make_shared<Session>();
  m_isConnSuccess = false;

  auto st = steady_clock::now();
  while (!m_connector.TryConnect(
      std::bind(&NetworkAgent::OnConnSuccess, this, std::placeholders::_1),
      std::bind(&NetworkAgent::OnConnFail, this, std::placeholders::_1))) {
    auto en = steady_clock::now();
    if (duration_cast<milliseconds>(en - st) > timeOut) {
      break;
    }
  }
  if (!m_isConnSuccess) {
    return nullptr;
  }
  auto returnSession = std::move(m_connSession);
  m_connSession = nullptr;
  return returnSession;
}

void NetworkAgent::OnRecv(sh::IO_Engine::TCP_ISessionPtr session, uint64_t packetSize, BYTE* packetPtr) {
}

void NetworkAgent::OnConnSuccess(SOCKET sock) {
  using namespace std::placeholders;
  WRITE_LOG(LogLevel::debug, "{}({})> Connect Success", __FUNCTION__, __LINE__);
  m_isConnSuccess = true;
  m_connSession->DefferedSet(sock, std::bind(NetworkAgent::OnRecv, _1, _2, _3), m_core.GetHandle());
}

void NetworkAgent::OnConnFail(int errorCode) {
  WRITE_LOG(LogLevel::debug, "{}({})> Connect Fail, ErrorCode: {}", __FUNCTION__, __LINE__, errorCode);
  m_isConnSuccess = false;
  m_connSession = nullptr;
}
}  // namespace SimpleTCP
