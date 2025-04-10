#pragma once
#include <memory>

namespace Stress {
class Session;
using sessionPtr = std::shared_ptr<Session>;
class CharacterObject;

class StressFSMBase
    : public std::enable_shared_from_this<StressFSMBase> {
 public:
  StressFSMBase(sessionPtr& session)
      : m_session(session) {
  }

  virtual void EnterState() = 0;
  virtual void UpdateState() = 0;
  virtual void ExitState() = 0;

 protected:
  std::weak_ptr<Session> m_session;
};

class LoginState
    : public StressFSMBase {
 public:
  LoginState(sessionPtr& session)
      : StressFSMBase(session) {
  }

  virtual void EnterState() override;
  virtual void UpdateState() override;
  virtual void ExitState() override;
};

class MatchState
    : public StressFSMBase {
 public:
  MatchState(sessionPtr& session)
      : StressFSMBase(session) {
  }

  virtual void EnterState() override;
  virtual void UpdateState() override;
  virtual void ExitState() override;
};

class InGameState
    : public StressFSMBase {
 public:
  InGameState(sessionPtr& session)
      : StressFSMBase(session) {
  }

  virtual void EnterState() override;
  virtual void UpdateState() override;
  virtual void ExitState() override;

 private:
  std::shared_ptr<CharacterObject> m_character;
};

}  // namespace Stress
