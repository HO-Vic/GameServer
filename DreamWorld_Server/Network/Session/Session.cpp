#include "stdafx.h"
#include "Session.h"
#include "SessionMananger.h"

namespace DreamWorld {
Session::Session()
    : m_uniqueNo(0) {
}

Session::Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, uint32_t uniqueNo)
    : sh::IO_Engine::ISession(sock, ioType, recvHandler, iocpHandle), m_uniqueNo(uniqueNo) {
}

void Session::Disconnect() {
  SessionMananger::GetInstance().OnDisconnect(m_uniqueNo);
}

const uint32_t Session::GetUniqueNo() const {
  return m_uniqueNo;
}

void Session::SetIngameRef(std::shared_ptr<RoomBase>& roomRef, std::shared_ptr<CharacterObject>& characterRef) {
  m_roomWeakRef = roomRef;
  m_possessCharacter = characterRef;
}

}  // namespace DreamWorld