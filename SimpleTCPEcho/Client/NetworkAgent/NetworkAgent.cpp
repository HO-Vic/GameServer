#include "pch.h"
#include "NetworkAgent.h"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <WinSock2.h>
#include <type_traits>
#include <IO_Engine/Session/TCP_SessionBase.h>
#include "../Session/Session.h"
#include "../LogMgr/LogManager.h"
#include "../Packet/Packet.h"
#include "MsgDispatcher.h"

namespace SimpleTCP {
NetworkAgent::NetworkAgent(const uint8_t ioThreadNo, const std::string& ipAddr, const uint16_t portAddr)
    : m_core(ioThreadNo), m_connector(ipAddr, portAddr), m_connSession(nullptr), m_isConnSuccess(false) {
}

void NetworkAgent::Init() {
  m_core.Init();
  MsgInit();
}

void NetworkAgent::Start() {
  m_core.Start();
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

void NetworkAgent::OnRecv(SessionPtr session, uint64_t packetSize, BYTE* packetPtr) {
  auto packetHeader = reinterpret_cast<PacketHeader*>(packetPtr);
  MsgHandle msgHandle = nullptr;
  if (!m_dispatcher.GetHandle(packetHeader->type, msgHandle)) {  // 타입에 맞는 패킷을 못찾으면, 수행ㄴㄴ
    WRITE_LOG(spdlog::level::info, "{}({}) > Unknown packet", __FUNCTION__, __LINE__);
    return;
  }
#ifdef _DEBUG
  assert(nullptr != msgHandle);
#endif  // _DEBUG
  msgHandle(session, packetPtr);
  // msgHandle(session, packetHeader);
}

void NetworkAgent::OnSimpleMsg(SessionPtr sessionPtr, BYTE* packetHeader) {
  auto msgPacket = reinterpret_cast<SimpleMsgPacket*>(packetHeader);
  SimpleMsg simpleMsg{};
  simpleMsg.Deserialize(msgPacket);
  WRITE_LOG(LogLevel::info, "RecvComplete [Payload:{}], MSG: {}", simpleMsg.GetPayloadMsg().size(), simpleMsg.GetPayloadMsg().c_str());
}

void NetworkAgent::OnParsedMsg(SessionPtr sessionPtr, BYTE* packetHeader) {
  auto msgPacket = reinterpret_cast<ParseMsgPacket*>(packetHeader);
  ParsedMsg parsedMsg{};
  parsedMsg.Deserialize(msgPacket);
  auto& payload = parsedMsg.GetPayload();
  uint32_t msgSize = 0;
  for (const auto& data : payload) {
    WRITE_LOG(LogLevel::info, "RecvComplete Parsed MSG: {}", data);
    msgSize += static_cast<uint32_t>(data.size());
  }
  WRITE_LOG(LogLevel::info, "RecvComplete [Payload:{}]", msgSize);
}

void NetworkAgent::OnConnSuccess(SOCKET sock) {
  using namespace std::placeholders;
  WRITE_LOG(LogLevel::debug, "{}({})> Connect Success", __FUNCTION__, __LINE__);
  m_isConnSuccess = true;
  auto h = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), m_core.GetHandle(), sock, 0);
#ifdef _DEBUG
  assert(h != NULL);
#endif  // _DEBUG
  m_connSession->DefferedSet(sock, std::bind(&NetworkAgent::OnRecv, this, _1, _2, _3), m_core.GetHandle());
  m_connSession->StartRecv();
}

void NetworkAgent::OnConnFail(int errorCode) {
  WRITE_LOG(LogLevel::debug, "{}({})> Connect Fail, ErrorCode: {}", __FUNCTION__, __LINE__, errorCode);
  m_isConnSuccess = false;
  m_connSession = nullptr;
}
}  // namespace SimpleTCP
