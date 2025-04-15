#pragma once
#include "../PCH/stdafx.h"

namespace DreamWorld {
class Session;
class MatchQueue {
 public:
  MatchQueue();
  ~MatchQueue() = default;

 public:
  std::optional<std::shared_ptr<Session>> GetMatchUser();
  void InsertMatchUser(std::shared_ptr<Session>& userRef);
  void InsertCancelUser(std::shared_ptr<Session>& userRef);

  const size_t QueueSize() const {
    return m_match.unsafe_size();
  }

 private:
  // 취소한 유저가 없을 때, weak_ptr nullptr이 없음
  std::optional<std::shared_ptr<Session>> GetCancelUser();

 private:
  tbb::concurrent_queue<std::weak_ptr<Session>> m_match;
  tbb::concurrent_queue<std::weak_ptr<Session>> m_cancel;

  std::weak_ptr<Session> m_lastCancelUserWeakPtr;
};
}  // namespace DreamWorld
