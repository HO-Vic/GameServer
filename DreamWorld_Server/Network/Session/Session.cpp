#include "stdafx.h"
#include "Session.h"
#include "SessionMananger.h"
#include "../LogManager/LogManager.h"
#include "../Room/RoomThreadPool.h"
#include "../Room/RoomBase.h"
#include "../Room/Room.h"
#include "../ObjectPools.h"

namespace DreamWorld {
Session::Session()
    : m_uniqueNo(0) {
}

Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, uint32_t uniqueNo)
    : sh::IO_Engine::ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo) {
}

Session::~Session() {
  WRITE_LOG(logLevel::trace, "{}({}) > Call Session Destructor [playerUniqueId:{}]", __FUNCTION__, __LINE__, m_uniqueNo);
}

void Session::OnDisconnect() {
  SessionMananger::GetInstance().OnDisconnect(m_uniqueNo);
  WRITE_LOG(logLevel::trace, "{}({}) > player Disconn! [playerUniqueId:{}]", __FUNCTION__, __LINE__, m_uniqueNo);
  auto roomPtr = m_roomWeakRef.lock();
  if (nullptr == roomPtr) {
    return;
  }
  // OnGameEndOk 패킷에서도 DiscardPlayer 호출 -> Room에서 session에 대한 strong 없어짐 -> 해당 부분 룸에 인서트(비동기적 수행) -> session delete -> shared_from_this()는 bad_weak일 수 밖에
  // 안전하게 복사 캡쳐로 넘겨서 strong 유지
  // 차라리 session::m_uniqueId 넘기는게 안전할듯ㅠ
  auto sessionPtr = std::static_pointer_cast<DreamWorld::Session>(shared_from_this());
  roomPtr->InsertJob(
      DreamWorld::ObjectPool<sh::Utility::Job>::GetInstance().MakeUnique([=]() {
        roomPtr->DiscardPlayer(sessionPtr);
        // roomPtr->DiscardPlayer(std::static_pointer_cast<DreamWorld::Session>(shared_from_this()););// 기존 코드
      }));
}

const uint32_t Session::GetUniqueNo() const {
  return m_uniqueNo;
}

void Session::SetIngameRef(std::shared_ptr<RoomBase>& roomRef, std::shared_ptr<CharacterObject>& characterRef) {
  m_roomWeakRef = roomRef;
  m_possessCharacter = characterRef;
}

}  // namespace DreamWorld