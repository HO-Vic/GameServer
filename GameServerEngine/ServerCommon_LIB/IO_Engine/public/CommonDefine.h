#include <functional>
namespace sh::IO_Engine {

#pragma region TYPE
enum IO_TYPE : char {
  TCP = 1,
  UDP = 2,
};

enum OVERLAPPED_EVENT_TYPE : char {
  SEND = 1,
  RECV = 2,
  TERMINATE = 3,
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
#pragma endregion

}  // namespace sh::IO_Engine