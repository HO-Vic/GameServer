#pragma once
#include "../SingletonBase.h"
#include "MatchQueue.h"
#include "../DreamWorldDefine.h"

namespace DreamWorld {
class Session;
class Matching : public SingletonBase<Matching> {
 public:
  void InsertMatch(std::shared_ptr<Session>& userRef, const ROLE role);
  void CancelMatch(std::shared_ptr<Session>& userRef, const ROLE role);

  void StartMatching();

 private:
  void MatchFunc();

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
};
}  // namespace DreamWorld
