#include <Windows.h>
#include <functional>
#include <WinSock2.h>
namespace sh::IO_Engine {
#ifndef IO_ENGINE_DEFINE
#define IO_ENGINE_DEFINE
#pragma region TYPE
enum IO_TYPE : char {
  TCP = 1,
  UDP = 2,
};

enum OVERLAPPED_EVENT_TYPE : char {
  NONE = 0,
  SEND = 1,
  RECV = 2,
  TERMINATE = 3,
  ACCEPT = 4,
  DISCONNECT = 5,
};
#pragma endregion

#pragma region PACKET
struct PacketHeader {
  uint16_t size;
};

constexpr static uint32_t MAX_RECV_BUF_SIZE = 1024;
#pragma endregion

#pragma region FUNC_DEFINE
using RecvHandler = std::function<void(size_t, BYTE*)>;
using AcceptCompleteHandler = std::function<void(SOCKET)>;
#pragma endregion

struct ConnectInfo {
  char localInfo[sizeof(SOCKADDR) + 16];
  char remoteInfo[sizeof(SOCKADDR) + 16];
  ConnectInfo() {
    ZeroMemory(localInfo, sizeof(SOCKADDR) + 16);
    ZeroMemory(remoteInfo, sizeof(SOCKADDR) + 16);
  }
};
#endif  // !IO_ENGINE_DEFINE

}  // namespace sh::IO_Engine