namespace SimpleTCP {
enum PACKET_TYPE : char {
  SIMPLE_MSG = 1
};
#pragma pack(push, 1)

struct PacketHeader {
  PacketHeader() {
  }
  PACKET_TYPE type;
};

struct SimpleMsgPacket : public PacketHeader {
  SimpleMsgPacket() {
  }
  uint32_t msgSize = 0;
};

struct ParsedMsg {
  uint32_t msgSize;
};

struct SimpleMsgParsePacket : public PacketHeader {
  SimpleMsgParsePacket() {
  }
  uint32_t msgCnt = 0;
};
#pragma pack(pop)

class SimpleMsg {
  std::string msg;
  SimpleMsg(const std::string& str)
      : msg(str) {
  }



};

}  // namespace SimpleTCP