#include "PCH.h"
#include "PCH.h"
#include "Packet.h"
#include <cassert>

namespace SimpleTCP {
PacketHeader::PacketHeader(PACKET_TYPE t)
    : type(t) {
}

SimpleMsgPacket::SimpleMsgPacket()
    : PacketHeader(PACKET_TYPE::SIMPLE_MSG), msgSize(0), isHostByte(true) {
}

SimpleMsgPacket::SimpleMsgPacket(uint32_t msgLen, const bool hostByte /*= true*/)
    : PacketHeader(PACKET_TYPE::SIMPLE_MSG), msgSize(msgLen), isHostByte(hostByte) {
}

void SimpleMsgPacket::Serialize() {
#ifdef _DEBUG
  assert(true == isHostByte);
#endif  // _DEBUG

  msgSize = htonl(msgSize);
  isHostByte = false;
}

void SimpleMsgPacket::Deserilaize() {
#ifdef _DEBUG
  assert(false == isHostByte);
#endif  // _DEBUG
  msgSize = ntohl(msgSize);
  isHostByte = true;
}

char* SimpleMsgPacket::GetMsgPtr() const {  // 반드시, 호스트 바이트 상태로 호출을..
#ifdef _DEBUG
  assert(true == isHostByte);
#endif  // _DEBUG
  auto msgPayloadOffset = sizeof(SimpleMsgPacket);
  return const_cast<char*>(reinterpret_cast<const char*>(this)) + msgPayloadOffset;
}

SimpleMsg::SimpleMsg(std::span<const char> data)
    : msg(data.data()), serializeSize{static_cast<uint32_t>(msg.size() + sizeof(SimpleMsgPacket))} {  // 문자열+크기+널문자+패킷헤더
  // 멤버 필드 msg가 serializeSize보다 앞에 위치하니, 멤버 이닛 리스트에서 .size()호출해도 문제xx
}

SimpleMsg::SimpleMsg(const std::string& data)
    : msg(data), serializeSize{static_cast<uint32_t>(msg.size() + sizeof(SimpleMsgPacket))} {
}

SimpleMsg::SimpleMsg(std::string&& data)
    : msg(std::move(data)), serializeSize{static_cast<uint32_t>(msg.size() + sizeof(SimpleMsgPacket))} {
}

void SimpleMsg::Serialize(BYTE* writePtr, const uint32_t writeAbleSize) const {
#ifdef _DEBUG
  if (GetSerializeSize() <= writeAbleSize) {
    assert(GetSerializeSize() <= writeAbleSize);
  }
#endif  // _DEBUG

  uint32_t msgSize = static_cast<uint32_t>(msg.size());
  SimpleMsgPacket tempPacket{msgSize, true};
  tempPacket.Serialize();
  // 패킷 타입
  int32_t writeSize = 0;
  writePtr[0] = PACKET_TYPE::SIMPLE_MSG;
  memcpy_s(writePtr + writeSize, static_cast<int64_t>(writeAbleSize - writeSize), &tempPacket, sizeof(SimpleMsgPacket));
  writeSize += sizeof(SimpleMsgPacket);

  // 메세지
  memcpy_s(writePtr + writeSize, static_cast<int64_t>(writeAbleSize - writeSize), msg.c_str(), msgSize);
  writeSize += msgSize;
}

void SimpleMsg::Deserialize(SimpleMsgPacket* msgPacket) {
  msgPacket->Deserilaize();
  msg.resize(msgPacket->msgSize);
  memcpy_s(msg.data(), msg.size(), msgPacket->GetMsgPtr(), msgPacket->msgSize);
  serializeSize = static_cast<uint32_t>(msg.size() + sizeof(SimpleMsgPacket));
}

PK_ParsedMsg::PK_ParsedMsg(uint16_t msgLen, bool isHost /*= true*/)
    : msgSize(msgLen), isHostByte(isHost) {
}

void PK_ParsedMsg::Serialize() {
  msgSize = htons(msgSize);
}

void PK_ParsedMsg::Deserilaize() {
  msgSize = ntohs(msgSize);
}

ParseMsgPacket::ParseMsgPacket(uint32_t msgCnt, bool isHost /*= true*/)
    : PacketHeader(PACKET_TYPE::PARSED_MSG), parsedMsgCnt(msgCnt), isHostByte(isHost) {
}

void ParseMsgPacket::Serialize() {
  parsedMsgCnt = htonl(parsedMsgCnt);
}

void ParseMsgPacket::Deserilaize() {
  parsedMsgCnt = ntohl(parsedMsgCnt);
}

void ParsedMsg::Serialize(BYTE* writePtr, const uint32_t writeAbleSize) const {
#ifdef _DEBUG
  if (GetSerializeSize() <= writeAbleSize) {
    assert(GetSerializeSize() <= writeAbleSize);
  }
#endif  // _DEBUG
  uint32_t writeSize = 0;

  //// 내부 메세지 갯수에 대한
  ParseMsgPacket pk{static_cast<uint32_t>(msgs.size())};
  pk.Serialize();
  memcpy_s(writePtr + writeSize, static_cast<int64_t>(writeAbleSize - writeSize), &pk, sizeof(ParseMsgPacket));
  writeSize += sizeof(ParseMsgPacket);

  for (const auto& msg : msgs) {
    // 현재 매세지의 길이
    PK_ParsedMsg pk_parsed{static_cast<uint16_t>(msg.size())};
    pk_parsed.Serialize();
    memcpy_s(writePtr + writeSize, static_cast<int64_t>(writeAbleSize - writeSize), &pk_parsed, sizeof(PK_ParsedMsg));
    writeSize += sizeof(PK_ParsedMsg);

    // 실제 메세지 내용
    memcpy_s(writePtr + writeSize, static_cast<int64_t>(writeAbleSize - writeSize), msg.c_str(), msg.size());
    writeSize += static_cast<uint32_t>(msg.size());
  }
}

void ParsedMsg::Deserialize(ParseMsgPacket* parsedMsgPkt) {
  parsedMsgPkt->Deserilaize();
  auto msgCnt = parsedMsgPkt->parsedMsgCnt;
  msgs.resize(msgCnt);
  char* readPosition = reinterpret_cast<char*>(parsedMsgPkt) + sizeof(ParseMsgPacket);
  for (uint32_t i = 0; i < msgCnt; ++i) {
    auto parsedMsgPtr = reinterpret_cast<PK_ParsedMsg*>(readPosition);
    parsedMsgPtr->Serialize();
    auto msgSize = parsedMsgPtr->msgSize;
    readPosition = readPosition + sizeof(PK_ParsedMsg);
    msgs[i].resize(msgSize);
    memcpy_s(msgs[i].data(), msgSize, readPosition, msgSize);
    readPosition += msgSize;
    //
    serializeSize += msgSize + sizeof(PK_ParsedMsg);
  }
}
}  // namespace SimpleTCP