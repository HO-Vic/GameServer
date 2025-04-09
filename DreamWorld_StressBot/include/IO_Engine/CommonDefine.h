#include <Windows.h>
#include <functional>
#include <WinSock2.h>
#include <memory>

namespace sh::IO_Engine {
#ifndef IO_ENGINE_DEFINE
#define IO_ENGINE_DEFINE
#pragma region TYPE
enum IO_TYPE : char {
  TCP = 1,
  UDP = 2,
};

#pragma endregion

#pragma region PACKET
#pragma pack(push, 1)
struct PacketHeader {
  uint16_t size;
  PacketHeader(const uint16_t size)
      : size(size) {
  }
};
#pragma pack(pop)

constexpr static uint32_t MAX_RECV_BUF_SIZE = 1024;
#pragma endregion

#pragma region FUNC_DEFINE
class ISession;
using ISessionPtr = std::shared_ptr<ISession>;
using RecvHandler = std::function<void(ISessionPtr, size_t, BYTE*)>;
using AcceptCompleteHandler = std::function<void(SOCKET)>;

using ConnectCompleteHandler = std::function<void(SOCKET)>;
using ConnectFailHandler = std::function<void(int)>;
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