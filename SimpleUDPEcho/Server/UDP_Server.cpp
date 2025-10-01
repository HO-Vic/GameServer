#include "pch.h"
#include "UDP_Server.h"
#include <Winsock2.h>
#include <cstdint>
#include <memory>
#include <functional>
#include "../Session/UDP_Agent/UDP_Agent.h"
#include "../LogMgr/LogManager.h"
#include <IO_Engine/CommonDefine.h>
#include <Windows.h>
#include "MsgProtocol.h"
#include "../SessionManager/SessionManager.h"
#include "../Session/UDP_Session/UDP_Session.h"

namespace sh::UDP_Echo {
void UDP_Server::Start() {
  m_ioCore.Start();
  WRITE_LOG(level::info, "{} ({}) > Server Start", __FUNCTION__, __LINE__);
  while (true) {
  }
}

bool UDP_Server::Init(const uint8_t ioThreadNo, const uint32_t thWorkerPoolSize, const uint32_t sendBufferPoolSize) {
  m_ioCore.Init(ioThreadNo);

  auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sockaddr_in addrInfo{};
  addrInfo.sin_family = AF_INET;
  addrInfo.sin_addr.s_addr = htonl(INADDR_ANY);
  addrInfo.sin_port = htons(9000);
  auto bindResult = bind(sock, reinterpret_cast<sockaddr*>(&addrInfo), sizeof(sockaddr_in));
  if (SOCKET_ERROR == bindResult) {
    WRITE_LOG(level::critical, "{} ({}) > socket bind fail", __FUNCTION__, __LINE__);
    return false;
  }

  auto handle = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), m_ioCore.GetHandle(), static_cast<ULONG_PTR>(sock), 0);
  if (NULL == handle) {
    WRITE_LOG(level::critical, "{} ({}) > can not regist sock to ioHandle", __FUNCTION__, __LINE__);
    assert(NULL != handle);
  }

  // recv객체를 n(n>1)개로 두는 경우, UDP 특성뿐 아니라, 어플리케이션 레이어에서도 순서 바뀜 가능
  // seq: 0 recv했지만, sleep -> seq: 1 recv 및 처리하면 seq 1이 우선 처리 됨. **주의**
  m_agent = std::make_shared<UDP_Agent>(sock, 2, 9000);
  WRITE_LOG(level::info, "{} ({}) > UDP sock Init", __FUNCTION__, __LINE__);

  using namespace std::placeholders;
  m_agent->StartRecv(std::bind(&UDP_Server::RecvHandler, this, _1, _2, _3, _4));
  WRITE_LOG(level::info, "{} ({}) > UDP Server recv Start", __FUNCTION__, __LINE__);

  WRITE_LOG(level::info, "{} ({}) > Server Init, ioThreadNo: {}", __FUNCTION__, __LINE__, ioThreadNo);

  return true;
}
// hashkey로 SessionMgr::sessions hash map 접근할 때마다 lock은 구릴듯
// 이건 agentPtr(recv 주체?)를 기준으로 Session 샤딩해서 저장하는 것도 방법중 하나일듯?
// 지금은 미사용
void UDP_Server::RecvHandler(IO_Engine::UDP_IAgentPtr agentPtr, [[maybe_unused]] size_t ioByte, BYTE* bufferPtr, const sockaddr_in& addrInfo) {
  auto packetPtr = reinterpret_cast<PacketHeader*>(bufferPtr);
  auto hashKey = UDP_Session::GetHash(addrInfo);
  switch (packetPtr->type) {
    case TYPE::ConnReq: {
#ifdef _DEBUG
      auto ipInfo = inet_ntoa(addrInfo.sin_addr);
      WRITE_LOG(level::info, "{}({}) > connInfo ip: {}, port: {}", __FUNCTION__, __LINE__, ipInfo, ntohs(addrInfo.sin_port));
#endif  // _DEBUG

      auto sessionPtr = SessionManager::GetInstance().InsertSession(hashKey, addrInfo);
      ConnAns ans{};
      sessionPtr->DoSend(agentPtr, reinterpret_cast<BYTE*>(&ans), sizeof(ConnAns));
      break;
    }
    case TYPE::PingReq: {
      auto sessionPtr = SessionManager::GetInstance().GetSession(hashKey);
      if (nullptr == sessionPtr) {
        WRITE_LOG(level::warn, "{}({}) > can not find session", __FUNCTION__, __LINE__);
        return;
      }
      ConnAns ans{};
      sessionPtr->DoSend(agentPtr, reinterpret_cast<BYTE*>(&ans), sizeof(ConnAns));
      break;
    }

    case TYPE::MSGPacket: {
      const auto* const msgPtr = reinterpret_cast<MsgPacket*>(bufferPtr);
      WRITE_LOG(level::info, "MSG recv, seq: {}, msg: {}", msgPtr->seq, msgPtr->msgBuffer);

      auto sessionPtr = SessionManager::GetInstance().GetSession(hashKey);
      sessionPtr->DoSend(agentPtr, bufferPtr, msgPtr->size);  // 에코 서버니 그대로 보냄
      break;
    }
    default:
      break;
  }
}
}  // namespace sh::UDP_Echo