#include <WinSock2.h>
#include <Windows.h>
#include <functional>
#include <memory>
#include <cstdint>
#include <utility>

#ifndef IO_ENGINE_DEFINE
#define IO_ENGINE_DEFINE
namespace sh::IO_Engine {
#pragma region TYPE
enum class IO_TYPE : char {
  TCP = 1,
  UDP = 2,
};

enum class ErrorType : char {
  None = 0,
  WIN32_ERR = 1,             // GetLastError()
  WINSOCK_ERR = 2,           // WsaGetLastError()
  IO_CORE_NOT_INIT = 3,      // IO_Core non Init
  FailedDisconnectEx = 4,    // DisconnectEx load fail - WSAGetLastError()
  IO_CORE_ALREADY_INIT = 5,  // IO_Core Already Init
};

// <ErrorType, detail error Code>
using ErrorResult = std::pair<ErrorType, int>;

#pragma endregion

#pragma region PACKET
#pragma pack(push, 1)
struct PacketHeader {
  PacketHeader()
      : size(0) {};
  PacketHeader(const uint16_t parSize)
      : size(parSize) {
  }
  // 링버퍼에서 서로 다른 위치의 packetSize를 해결하기 위해
  PacketHeader(const char* first, const char* second)
      : size(0) {
    size = static_cast<uint16_t>(static_cast<uint8_t>(*first)) << 8;
    size = (size | static_cast<uint8_t>(*second));
  }
  PacketHeader(const BYTE* first, const BYTE* second)
      : size(0) {
    size = static_cast<uint16_t>(static_cast<uint8_t>(*first)) << 8;
    size = (size | static_cast<uint8_t>(*second));
  }
  void Serialize() {
    size = htons(size);
  }
  void Deserialize() {
    size = ntohs(size);
  }
  uint16_t size;
};
#pragma pack(pop)

constexpr static uint32_t MAX_RECV_BUF_SIZE = 2000;
#pragma endregion

#pragma region FUNC_DEFINE
class TCP_SessionBase;
class UDP_SessionBase;
class UDP_AgentBase;
using TCP_SessionBasePtr = std::shared_ptr<TCP_SessionBase>;
using UDP_SessionBasePtr = std::shared_ptr<UDP_SessionBase>;
using UDP_AgentBasePtr = std::shared_ptr<UDP_AgentBase>;
using TCP_RecvHandler = std::function<void(TCP_SessionBasePtr, size_t, BYTE*)>;
using UDP_RecvHandler = std::function<void(UDP_AgentBasePtr, size_t, BYTE*, const sockaddr_in&)>;

class AcceptEvent;
using AcceptEventPtr = std::shared_ptr<AcceptEvent>;

// completion key를 필요로 하는경우, acceptComplete에서 iocp에 등록해야합니다
using AcceptCompleteHandler = std::function<void(SOCKET, AcceptEventPtr&&)>;

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
#pragma region Config

struct SocketConfig {
  int32_t inetType = AF_INET;
  int32_t socketType = SOCK_STREAM;
  int32_t protocolType = IPPROTO_TCP;
};

struct AddrConfig {
  uint16_t port{};
  std::string ip = "0.0.0.0";
};
#pragma endregion

}  // namespace sh::IO_Engine
#endif  // !IO_ENGINE_DEFINE