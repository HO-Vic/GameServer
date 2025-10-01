#pragma once
#include <IO_Engine/CommonDefine.h>
#include <algorithm>

namespace sh::UDP_Echo {
enum class TYPE : unsigned char {
  ConnReq = 1,

  PingReq = 10,

  //

  MSGPacket = 50,

  ConnAns = 128,
  PingAns = 137,
};

#pragma pack(push, 1)

struct PacketHeader
    : public IO_Engine::PacketHeader {
  TYPE type;
  PacketHeader(const uint16_t size, TYPE parType)
      : IO_Engine::PacketHeader(size), type(parType) {
  }
};

// 간단하게 연결만 할거기 때문에, 다른 정보는 없음
struct ConnReq
    : public PacketHeader {
  ConnReq()
      : PacketHeader(3, TYPE::ConnReq) {
  }
};
struct ConnAns
    : public PacketHeader {
  ConnAns()
      : PacketHeader(3, TYPE::ConnAns) {
  }
};

struct PingReq
    : public PacketHeader {
  PingReq()
      : PacketHeader(3, TYPE::PingReq) {
  }
};

struct PingAns
    : public PacketHeader {
  PingAns()
      : PacketHeader(3, TYPE::PingAns) {
  }
};

struct MsgPacket
    : public PacketHeader {
  MsgPacket(uint16_t seqNo, const char* const msg, uint16_t msgLen)
      : PacketHeader(3 + msgLen + 1 + 2, TYPE::MSGPacket), seq(seqNo) {  // uint16_t + char + msg len + buff[msglen] + uint16_t
    memcpy_s(msgBuffer, 1023, msg, msgLen);
    msgLen = std::min<uint16_t>(msgLen, 1023);
    msgBuffer[msgLen] = 0;
  }
  uint16_t seq = 0;
  char msgBuffer[1024] = {0};
};

#pragma pack(pop)
}  // namespace sh::UDP_Echo