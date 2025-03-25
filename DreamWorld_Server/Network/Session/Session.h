#pragma once
#include <cstdint>
#include "IO_Engine/Session/ISession.h"
#include "../DreamWorldDefine.h"

namespace DreamWorld {
class RoomBase;
class CharacterObject;
class Session
    : public sh::IO_Engine::ISession {
 public:
  Session();

  Session(SOCKET sock, const sh::IO_Engine::IO_TYPE ioType, sh::IO_Engine::RecvHandler recvHandler, HANDLE iocpHandle, uint32_t uniqueNo);

  virtual void Disconnect() override;

  const uint32_t GetUniqueNo() const;

  void LoginSuccess(const wchar_t* nickName) {
    m_playerName = nickName;
  }

  const std::wstring& GetPlayerName() const {
    return m_playerName;
  }

  void SetIngameRole(const ROLE& role) {
    m_ingameRole = role;
  }

  const ROLE GetIngameRole() const {
    return m_ingameRole;
  }

  void SetName(std::string::iterator beginIter, std::string::iterator endIter) {
    m_playerName.assign(beginIter, endIter);
  }

  void SetName(std::wstring&& right) {
    m_playerName = right;
  }

  void SetIngameRef(std::shared_ptr<RoomBase>& roomRef, std::shared_ptr<CharacterObject>& characterRef);

  std::shared_ptr<CharacterObject> GetPossessCharacter() {
    return m_possessCharacter.lock();
  }

  std::shared_ptr<RoomBase> GetRoom() {
    return m_roomWeakRef.lock();
  }

 private:
  // INGame
  uint32_t m_uniqueNo;

  std::atomic<PLAYER_STATE> m_playerState;
  std::wstring m_playerName;

  std::weak_ptr<RoomBase> m_roomWeakRef;              // Room이 삭제 됐다면, 플레이어는 방에서 나오게
  std::weak_ptr<CharacterObject> m_possessCharacter;  // Room에서 플레이하고 있는 캐릭터

  ROLE m_ingameRole;   // 실제 인게임 ROLE
  ROLE m_matchedRole;  // 현재 match 큐에 삽입한 ROLE
};
}  // namespace DreamWorld
