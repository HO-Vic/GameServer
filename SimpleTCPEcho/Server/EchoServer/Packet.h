#pragma once
#include <span>
#include <string>

namespace SimpleTCP {
enum PACKET_TYPE : BYTE {
  SIMPLE_MSG = 1
};
#pragma pack(push, 1)

struct PacketHeader {
  PacketHeader() = default;
  PacketHeader(PACKET_TYPE t);

  PACKET_TYPE type = PACKET_TYPE::SIMPLE_MSG;
};

struct SimpleMsgPacket : public PacketHeader {
  SimpleMsgPacket();

  SimpleMsgPacket(uint32_t msgLen, const bool hostByte = true);

  void Serialize();

  void Deserilaize();

  char* GetMsgPtr() const;

  uint32_t msgSize = 0;
  bool isHostByte = true;
};

// struct ParsedMsg {
//   uint32_t msgSize;
// };
//
// struct SimpleMsgParsePacket : public PacketHeader {
//   SimpleMsgParsePacket() {
//   }
//   uint32_t parsedMsgCnt = 0;
// };
#pragma pack(pop)

class SimpleMsg {
 public:
  SimpleMsg() = default;

  SimpleMsg(std::span<const char> data);

  SimpleMsg(const std::string& data);

  SimpleMsg(std::string&& data);

  uint32_t GetSerializeSize() const {
    return serializeSize;
  }

  void Serialize(BYTE* writePtr, const uint32_t writeAbleSize) const;

  void Deserialize(SimpleMsgPacket* msgPacket);

  const std::string& GetPayloadMsg() const {
    return msg;
  }

 private:
  std::string msg;
  uint32_t serializeSize = 0;
};

}  // namespace SimpleTCP