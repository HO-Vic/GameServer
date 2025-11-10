#include "pch.h"
#include <fstream>
// #include <iostream>
// #include <WinSock2.h>
// #include <WS2tcpip.h>
// #include <vector>
// #include <assert.h>
//  #include "../EchoServer/Packet.h"

#include "Session/Session.h"
#include "LogMgr/LogManager.h"
#include "NetworkAgent/NetworkAgent.h"

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

  std::vector<std::string> datas;
  datas.reserve(100);
  std::ifstream in("../../data/echoTest.txt");
  while (true) {
    char readBuf[1024]{};
    in.getline(readBuf, 1024);
    datas.push_back(readBuf);
    if (in.eof()) {
      break;
    }
  }
  in.close();

  // 1. 단순 문자열 전송, 문자열은 각기 다른 길이를 가짐
  // 2. 문자열에 대해서 단어별 파싱(스페이스 기준)이걸 통으로 보내기, 즉, 어떤 메모리 layout으로 동작해야할지
  // 3. 모든 문자열을 한번에 전송시켜버리기
  // serialize/deserialize에 대한 공부
  for (const auto& data : datas) {
  }
}
