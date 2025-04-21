#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include "MatchQueue.h"
#include "../DreamWorldDefine.h"

namespace DreamWorld {
class Session;
class Matching : public sh::Utility::SingletonBase<Matching> {
 public:
  void Init(bool isLocal = false, uint8_t testPeople = 0);

  void InsertMatch(std::shared_ptr<Session>& userRef, const ROLE role);
  void CancelMatch(std::shared_ptr<Session>& userRef, const ROLE role);

  void StartMatching();

  const size_t GetWarriorSize() const {
    return m_warriorQueue.QueueSize();
  }

  const size_t GetMageSize() const {
    return m_mageQueue.QueueSize();
  }

  const size_t GetArcherSize() const {
    return m_archerQueue.QueueSize();
  }

  const size_t GetTankerSize() const {
    return m_tankerQueue.QueueSize();
  }

 private:
  void MatchFunc(std::stop_token stopToken);

  std::shared_ptr<Session> GetCurrentMatchUserSession(MatchQueue& matchQueue, std::shared_ptr<Session> lastUserSession);

 private:
  MatchQueue m_warriorQueue;
  MatchQueue m_archerQueue;
  MatchQueue m_mageQueue;
  MatchQueue m_tankerQueue;

  std::weak_ptr<Session> m_lastWarriorUser;
  std::weak_ptr<Session> m_lastMageUser;
  std::weak_ptr<Session> m_lastTankerUser;
  std::weak_ptr<Session> m_lastArcherUser;

  bool m_isLocal;
  uint8_t m_testPeople = 0;
};
}  // namespace DreamWorld
