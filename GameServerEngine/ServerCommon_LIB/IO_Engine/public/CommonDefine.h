#include <functional>
namespace sh::IO_Engine {
enum IO_TYPE : char {
  TCP = 1,
  UDP = 2
};

struct PacketHeader {
  uint16_t size;
};

constexpr static uint32_t MAX_RECV_BUF_SIZE = 1024;
using RecvHandler = std::function<void(size_t, BYTE*)>;

}  // namespace sh::IO_Engine