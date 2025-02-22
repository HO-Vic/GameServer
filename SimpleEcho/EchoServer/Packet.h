#include <IO_Engine/CommonDefine.h>

namespace sh::EchoServer {
enum PACKET_TYPE : char {
  SIMPLE_MSG = 1
};

struct PacketHeader
    : public IO_Engine::PacketHeader {
  PACKET_TYPE type;
};

struct SimpleMsgPacket : public PacketHeader {
  char msg[1024] = {0};
};

}  // namespace sh::EchoServer
