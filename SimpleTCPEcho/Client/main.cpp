#include "pch.h"
#include <fstream>
#include <span>
#include <cstring>
#include <vector>
#include <type_traits>
#include <IO_Engine/Buffer/SendBufferPool.h>
#include <IO_Engine/Buffer/SendBuffer.h>
#include <spdlog/common.h>
#include "LogMgr/LogManager.h"
#include "Session/Session.h"
// #include <IO_Engine/Session/TCP_ISession.h>
#include "NetworkAgent/NetworkAgent.h"
#include "Packet/Packet.h"

void ReadSimpleMsg(std::vector<SimpleTCP::SimpleMsg>& datas) {
  datas.reserve(30);
  std::ifstream in("../../data/echoTest2.txt");
  while (true) {
    char readBuf[2300]{};
    in.getline(readBuf, 2300);
    auto readSize = strlen(readBuf);
    if (readSize > 0) {
      datas.emplace_back(std::span<const char>(readBuf, readSize));
    }
    if (in.eof()) {
      break;
    }
  }
  in.close();
}
void ReadParseMsg(SimpleTCP::ParsedMsg& msg) {
  std::ifstream in("../../data/echoTest3.txt");
  while (true) {
    std::string str;
    in >> str;
    msg.Insert(std::move(str));
    if (in.eof()) {
      break;
    }
  }
  in.close();
}

int main() {
  START_LOGGER("EchoTcpClient", "Log", "Log", SimpleTCP::LogLevel::debug, "ConsoleFile");
  SimpleTCP::NetworkAgent netAgent{2, "127.0.0.1", 9000};

  netAgent.Init();
  auto session = netAgent.Connect(SimpleTCP::ms(1000));

  int limitConnTry = 5;
  while (nullptr == session && limitConnTry > 0) {
    session = netAgent.Connect(SimpleTCP::ms(1000));
    limitConnTry--;
  }
  if (nullptr == session) {
    WRITE_LOG(SimpleTCP::LogLevel::warn, "conn Fail");
    return 0;
  }
  netAgent.Start();

  std::vector<SimpleTCP::SimpleMsg> datas;
  ReadSimpleMsg(datas);
  std::reverse(datas.begin(), datas.end());
  SimpleTCP::ParsedMsg parsedMsg;
  ReadParseMsg(parsedMsg);

  while (true) {
    // 단어 가변 배열 전송
    auto packetSize = parsedMsg.GetSerializeSize();
    auto sendBuffer = sh::IO_Engine::SendBufferAllocator::GetInstance().GetShared(packetSize);
    auto writePtr = sendBuffer->GetWritePtr(packetSize);
    parsedMsg.Serialize(writePtr, packetSize);
    session->DoSend(std::move(sendBuffer));

    Sleep(100);

    // 가변 문자열들 전송
    for (const auto& msgData : datas) {
      // WRITE_LOG(SimpleTCP::LogLevel::info, "Send [PayLoadSize: {}] Send Msg: {}", msgData.GetPayloadMsg().size(), msgData.GetPayloadMsg());
      auto packetSize = msgData.GetSerializeSize();
      auto sendBuffer = sh::IO_Engine::SendBufferAllocator::GetInstance().GetShared(packetSize);
      auto writePtr = sendBuffer->GetWritePtr(packetSize);
      msgData.Serialize(writePtr, packetSize);
      session->DoSend(std::move(sendBuffer));
      Sleep(50);
    }

    Sleep(500);
    if (session->IsDisconnected()) {
      break;
    }
  }
  Sleep(1000);
  WRITE_LOG(SimpleTCP::LogLevel::info, "All Msg Send");
}
