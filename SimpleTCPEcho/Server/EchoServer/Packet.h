#pragma once
#include <span>
#include <string>

namespace SimpleTCP {
enum PACKET_TYPE : BYTE {
  SIMPLE_MSG = 1,
  PARSED_MSG = 2
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

  void Serialize();  // msgSize에 대한 ser/deser

  void Deserilaize();

  char* GetMsgPtr() const;

  uint32_t msgSize = 0;
  bool isHostByte = true;
};

struct PK_ParsedMsg {
  PK_ParsedMsg() = default;

  PK_ParsedMsg(uint16_t msgLen, bool isHost = true);

  void Serialize();  // msgSize에 대한 ser/deser

  void Deserilaize();

  uint16_t msgSize = 0;
  bool isHostByte = true;
};

struct ParseMsgPacket : public PacketHeader {
  ParseMsgPacket()
      : PacketHeader(PACKET_TYPE::PARSED_MSG), parsedMsgCnt(0), isHostByte(true) {
  }

  ParseMsgPacket(uint32_t msgCnt, bool isHost = true);

  void Serialize();  // parsedMsgCnt에 대한 ser/deser

  void Deserilaize();

  uint32_t parsedMsgCnt = 0;
  bool isHostByte = true;
};
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

class ParsedMsg {  // ws로 구분된 메세지 집합
 public:
  ParsedMsg() = default;

  ParsedMsg(const std::vector<std::string>& msg)
      : msgs(msg), serializeSize(sizeof(ParseMsgPacket)) {
    for (const auto& m : msgs) {
      serializeSize += static_cast<uint32_t>(sizeof(PK_ParsedMsg) + m.size());
    }
  }

  ParsedMsg(std::vector<std::string>&& msg)
      : msgs(std::move(msg)), serializeSize(sizeof(ParseMsgPacket)) {
    for (const auto& m : msgs) {
      serializeSize += static_cast<uint32_t>(sizeof(PK_ParsedMsg) + m.size());
    }
  }

  void Insert(const std::string& msg) {
    msgs.emplace_back(msg);
    serializeSize += static_cast<uint32_t>(sizeof(PK_ParsedMsg) + msg.size());
  }

  void Insert(std::string&& msg) {
    msgs.push_back(std::move(msg));
    serializeSize += static_cast<uint32_t>(sizeof(PK_ParsedMsg) + msgs.back().size());
  }

  uint32_t GetSerializeSize() const {
    return serializeSize;
  }

  const std::vector<std::string>& GetPayload() const {
    return msgs;
  }

  void Serialize(BYTE* writePtr, const uint32_t writeAbleSize) const;

  void Deserialize(ParseMsgPacket* parsedMsgPkt);

 private:
  std::vector<std::string> msgs;
  uint32_t serializeSize = sizeof(ParseMsgPacket);
};

}  // namespace SimpleTCP