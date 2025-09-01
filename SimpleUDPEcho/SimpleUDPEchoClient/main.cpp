#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <assert.h>
#include "../Server/MsgProtocol.h"
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
  WSADATA wsaData{};
  if (0 != WSAStartup(WINSOCK_VERSION, &wsaData)) {
#ifdef _DEBUG
    assert(false && "WSAData init fail");
#endif  // _DEBUG
  }

  std::string serverAddr = "127.0.0.1";

  SOCKADDR_IN server_addr;
  ZeroMemory(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9000);
  inet_pton(AF_INET, serverAddr.c_str(), &server_addr.sin_addr);

  SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == INVALID_SOCKET) {
    int aa = WSAGetLastError();
    if (aa != 0) {
      std::cerr << "conn error " << aa << "\n";
      return -1;
    }
  }
  sh::UDP_Echo::ConnReq connReq{};
  auto result = sendto(sock, reinterpret_cast<char*>(&connReq), connReq.size, 0, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(SOCKADDR_IN));

  if (result == SOCKET_ERROR) {
    cerr << "Raised Io Error At Send ConnReq: " << WSAGetLastError() << "\n";
    return -1;
  }

  cout << "send complete connReq, ioByte: " << result << endl;

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

  volatile bool end = false;
  volatile bool startAble = false;
  std::thread th([sock, server_addr, &startAble, &end]() {
    SOCKADDR_IN sockaddrInfo{};
    int sockAddrSize = sizeof(SOCKADDR_IN);
    while (!end) {
      char buffer[1400] = {0};
      auto result = recvfrom(sock, buffer, 1400, 0, reinterpret_cast<sockaddr*>(&sockaddrInfo), &sockAddrSize);
      if (result == SOCKET_ERROR) {
        cerr << "Recv Error: " << WSAGetLastError() << endl;
        break;
      }
      auto* packetHeader = reinterpret_cast<sh::UDP_Echo::PacketHeader*>(buffer);
      switch (packetHeader->type) {
        case sh::UDP_Echo::TYPE::ConnAns: {
          startAble = true;
          break;
        }
        // case sh::UDP_Echo::TYPE::PingAns: {
        //   //
        //   break;
        // };
        case sh::UDP_Echo::TYPE::MSGPacket: {
          auto* msgPacket = reinterpret_cast<sh::UDP_Echo::MsgPacket*>(buffer);
          cout << "msg Recv: " << msgPacket->msgBuffer << endl;
          break;
        }
        default:
          break;
      }
    }
  });
  while (!startAble) {
  }

  for (auto& str : sendBuffers) {
    sh::UDP_Echo::MsgPacket packet(str.c_str(), static_cast<uint16_t>(str.size()));
    sendto(sock, reinterpret_cast<char*>(&packet), packet.size, 0, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(SOCKADDR_IN));
  }

  th.join();

  closesocket(sock);
  std::cout << "end\n";
  getchar();
}
