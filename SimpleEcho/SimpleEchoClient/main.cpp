#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <assert.h>
#include "../EchoServer/Packet.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
  WSADATA wsaData{};
  if (0 != WSAStartup(WINSOCK_VERSION, &wsaData)) {
    assert("WSAData init fail");
  }

  std::string serverAddr = "127.0.0.1";

  SOCKADDR_IN server_addr;
  ZeroMemory(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9000);
  inet_pton(AF_INET, serverAddr.c_str(), &server_addr.sin_addr);

  SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connect(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
    int aa = WSAGetLastError();
    if (aa != 0) {
      std::cerr << "conn error\n";
    }
  }

  std::vector<std::string> sendBuffers;
  sendBuffers.reserve(100);
  std::ifstream in("echoTest.txt");
  while (true) {
    char readBuf[1024]{};
    in.getline(readBuf, 1024);
    sendBuffers.push_back(readBuf);
    if (in.eof()) {
      break;
    }
  }
  in.close();

  uint32_t sendCnt = 0;

  while (sendCnt != 100 - 1) {
    auto sendPacket = sh::EchoServer::SimpleMsgPacket{};
    sendPacket.type = sh::EchoServer::PACKET_TYPE::SIMPLE_MSG;
    memcpy(sendPacket.msg, sendBuffers[sendCnt].c_str(), sendBuffers[sendCnt].size());
    sendPacket.msg[sendBuffers[sendCnt].size()] = 0;
    sendPacket.size = sizeof(sh::EchoServer::PacketHeader) + sendBuffers[sendCnt].size() + 1;
    int sendErrCode = send(sock, reinterpret_cast<char*>(&sendPacket), sendPacket.size, 0);

    char recvBuf[1024]{};
    int recvErrCode = recv(sock, recvBuf, 1024, 0);
    sh::EchoServer::SimpleMsgPacket* recvPacket = reinterpret_cast<sh::EchoServer::SimpleMsgPacket*>(recvBuf);
    std::cout << recvPacket->msg << std::endl;
    sendCnt++;
  }
  closesocket(sock);
  std::cout << "end\n";
  getchar();
}
