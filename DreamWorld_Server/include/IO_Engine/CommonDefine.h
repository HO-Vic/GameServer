#include <WinSock2.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

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
  PacketHeader()
      : size(0) {};
  PacketHeader(const uint16_t parSize)
      : size(parSize) {
  }
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
inline thread_local std::string tlsString;
inline std::string& GetThreadString() {
  if (tlsString.size() < MAX_RECV_BUF_SIZE) {
    tlsString.resize(MAX_RECV_BUF_SIZE);
  }
  return tlsString;
}
#endif  // !IO_ENGINE_DEFINE

}  // namespace sh::IO_Engine