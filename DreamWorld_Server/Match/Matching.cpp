#include "stdafx.h"
#include "Matching.h"
#include "../Network/Session/Session.h"
#include "../Room/RoomManager.h"
#include "../Room/Room.h"
#include "../LogManager/LogManager.h"
#include "../ObjectPools.h"
#include "../Timer/TimerJob.h"
#include "../Timer/Timer.h"
#include "../Room/RoomThreadPool.h"
#include "../ThreadManager/ThreadManager.h"
#include <Utility/Thread/ThreadManager.h>

namespace DreamWorld {
void Matching::Init(bool isLocal /*=false*/, uint8_t testPeople /*=0*/) {
  m_isLocal = isLocal;
  m_testPeople = testPeople;
}
void Matching::InsertMatch(std::shared_ptr<Session>& userRef, const ROLE role) {
  if (m_isLocal) {
    spdlog::debug("Matching::InserMatch() - Alone Test Mode");
    userRef->SetIngameRole(role);
    auto roomRef = RoomManager::GetInstance().MakeRunningRoomAloneMode(userRef);
    auto characterPtr = roomRef->GetCharacter(role);
    if (nullptr == characterPtr) {
      WRITE_LOG(logLevel::err, "{}({}) > role is Invalid [role:{}]", __FUNCTION__, __LINE__, static_cast<char>(role));
      RoomManager::GetInstance().EraseRoom(roomRef);
      return;
    }
    auto roomBasePtr = std::static_pointer_cast<RoomBase>(roomRef);
    roomRef->Init();
    userRef->SetIngameRef(roomBasePtr, characterPtr);
    roomBasePtr->InsertPlayer(userRef);
    roomRef->StartGame();
    return;
  }
  switch (role) {
    case ROLE::WARRIOR:
      m_warriorQueue.InsertMatchUser(userRef);
      break;
    case ROLE::MAGE:
      m_mageQueue.InsertMatchUser(userRef);
      break;
    case ROLE::TANKER:
      m_tankerQueue.InsertMatchUser(userRef);
      break;
    case ROLE::ARCHER:
      m_archerQueue.InsertMatchUser(userRef);
      break;
    default:
      spdlog::warn("Matching::InserMatch() - Undefined Role");
      break;
  }
}

void Matching::CancelMatch(std::shared_ptr<Session>& userRef, const ROLE role) {
  switch (role) {
    case ROLE::WARRIOR:
      m_warriorQueue.InsertCancelUser(userRef);
      break;
    case ROLE::MAGE:
      m_mageQueue.InsertCancelUser(userRef);
      break;
    case ROLE::TANKER:
      m_tankerQueue.InsertCancelUser(userRef);
      break;
    case ROLE::ARCHER:
      m_archerQueue.InsertCancelUser(userRef);
      break;
    default:
      spdlog::warn("Matching::CacelMatch() - Undefined Role");
      break;
  }
}

void Matching::MatchFunc(std::stop_token stopToken) {
  static constexpr BYTE matchSuccessCondition = static_cast<BYTE>(ROLE::WARRIOR) |
                                                static_cast<BYTE>(ROLE::MAGE) |
                                                static_cast<BYTE>(ROLE::TANKER) |
                                                static_cast<BYTE>(ROLE::ARCHER);

  std::vector<std::shared_ptr<Session>> userRefVec;
  userRefVec.reserve(4);
  while (true) {
    if (stopToken.stop_requested()) {
      break;
    }
    BYTE matchState = 0;
    int currentMatchedUserCnt = 0;

    auto warriorUserRef = m_lastWarriorUser.lock();
    warriorUserRef = GetCurrentMatchUserSession(m_warriorQueue, warriorUserRef);
    if (nullptr != warriorUserRef) {
      matchState |= static_cast<BYTE>(ROLE::WARRIOR);
      userRefVec.emplace_back(warriorUserRef);
      ++currentMatchedUserCnt;
    }

    auto mageUserRef = m_lastMageUser.lock();
    mageUserRef = GetCurrentMatchUserSession(m_mageQueue, mageUserRef);
    if (nullptr != mageUserRef) {
      matchState |= static_cast<BYTE>(ROLE::MAGE);
      userRefVec.emplace_back(mageUserRef);
      ++currentMatchedUserCnt;
    }

    auto tankerUserRef = m_lastTankerUser.lock();
    tankerUserRef = GetCurrentMatchUserSession(m_tankerQueue, tankerUserRef);
    if (nullptr != tankerUserRef) {
      matchState |= static_cast<BYTE>(ROLE::TANKER);
      userRefVec.emplace_back(tankerUserRef);
      ++currentMatchedUserCnt;
    }

    auto archerUserRef = m_lastArcherUser.lock();
    archerUserRef = GetCurrentMatchUserSession(m_archerQueue, archerUserRef);
    if (nullptr != archerUserRef) {
      matchState |= static_cast<BYTE>(ROLE::ARCHER);
      userRefVec.emplace_back(archerUserRef);
      ++currentMatchedUserCnt;
    }

    if (0 != m_testPeople) {
      if (m_testPeople == currentMatchedUserCnt) {
        if (nullptr != warriorUserRef) {
          warriorUserRef->SetIngameRole(ROLE::WARRIOR);
          m_lastWarriorUser.reset();
        }
        if (nullptr != mageUserRef) {
          mageUserRef->SetIngameRole(ROLE::MAGE);
          m_lastMageUser.reset();
        }
        if (nullptr != tankerUserRef) {
          tankerUserRef->SetIngameRole(ROLE::TANKER);
          m_lastTankerUser.reset();
        }
        if (nullptr != archerUserRef) {
          archerUserRef->SetIngameRole(ROLE::ARCHER);
          m_lastArcherUser.reset();
        }

        //auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
        //roomRef->InitializeAllGameObject();
        //for (auto& userRef : userRefVec) {
        //  if (nullptr != userRef) {
        //    userRef->SetIngameRef(roomRef, roomRef->GetCharacterObject(userRef->GetIngameRole()));
        //  }
        //}
        //roomRef->Start();

        userRefVec.clear();
      } else {
        if (nullptr != warriorUserRef) {
          m_lastWarriorUser = warriorUserRef;
        }
        if (nullptr != mageUserRef) {
          m_lastMageUser = mageUserRef;
        }
        if (nullptr != tankerUserRef) {
          m_lastTankerUser = tankerUserRef;
        }
        if (nullptr != archerUserRef) {
          m_lastArcherUser = archerUserRef;
        }
        userRefVec.clear();
        std::this_thread::yield();
      }
      return;
    }
    if ((matchSuccessCondition & matchState) == matchSuccessCondition) {
      auto roomRef = RoomManager::GetInstance().MakeRunningRoom(userRefVec);
      if (nullptr == roomRef) {
        m_lastWarriorUser = warriorUserRef;
        m_lastMageUser = mageUserRef;
        m_lastTankerUser = tankerUserRef;
        m_lastArcherUser = archerUserRef;
        userRefVec.clear();
        std::this_thread::yield();
        continue;
      }
      // match Success
      warriorUserRef->SetIngameRole(ROLE::WARRIOR);
      mageUserRef->SetIngameRole(ROLE::MAGE);
      tankerUserRef->SetIngameRole(ROLE::TANKER);
      archerUserRef->SetIngameRole(ROLE::ARCHER);

      for (auto& userRef : userRefVec) {
        auto characterPtr = roomRef->GetCharacter(userRef->GetIngameRole());
        if (nullptr == characterPtr) {
          RoomManager::GetInstance().EraseRoom(roomRef);
          return;
        }
        userRef->SetIngameRef(std::static_pointer_cast<RoomBase>(roomRef), characterPtr);
        roomRef->InsertPlayer(userRef);
      }
      roomRef->Init();
      roomRef->StartGame();

      m_lastWarriorUser.reset();
      m_lastMageUser.reset();
      m_lastTankerUser.reset();
      m_lastArcherUser.reset();
      userRefVec.clear();
    } else {
      m_lastWarriorUser = warriorUserRef;
      m_lastMageUser = mageUserRef;
      m_lastTankerUser = tankerUserRef;
      m_lastArcherUser = archerUserRef;
      userRefVec.clear();
      std::this_thread::yield();
    }
  }
}

std::shared_ptr<Session> Matching::GetCurrentMatchUserSession(MatchQueue& matchQueue, std::shared_ptr<Session> lastUserSession) {
  std::shared_ptr<Session> returnUserSession = nullptr;
  returnUserSession = lastUserSession;
  while (true) {
    if (nullptr != returnUserSession) {
      return returnUserSession;
    } else {
      auto frontUserSession = matchQueue.GetMatchUser();
      if (frontUserSession.has_value()) {
        returnUserSession = frontUserSession.value();
        continue;
      } else {
        return nullptr;
      }
    }
  }
  return nullptr;
}

void Matching::StartMatching() {
  m_lastWarriorUser.reset();
  m_lastMageUser.reset();
  m_lastTankerUser.reset();
  m_lastArcherUser.reset();
  if (!m_isLocal) {
    spdlog::info("Matching::StartMatching() - Matching Thread Start");
    ThreadManager::GetInstance().InsertThread([](std::stop_token stopToken) {
      Matching::GetInstance().MatchFunc(stopToken);
    });
  } else {
    spdlog::info("Matching::StartMatching() - AloneTestMode, Matching Thread do not run");
  }
}
}  // namespace DreamWorld
