#pragma once
#include <memory>
#include <Windows.h>
#include "Utility/SingletonBase/Singleton.h"
#include "../MsgDispatcher/MsgDispatcher.h"

/*
Room에서 일어나야하는 이벤트 입니다.
*/

namespace sh::IO_Engine {
class TCP_ISession;
using TCP_ISessionPtr = std::shared_ptr<TCP_ISession>;
}  // namespace sh::IO_Engine

namespace DreamWorld {
class RoomMsgDispatcher final
    : public sh::Utility::SingletonBase<RoomMsgDispatcher>,
      public MsgDispatcher {
 public:
  void Init();  // 여기서 모든 msg Handler 넣어야 됨
 private:
  static void OnKeyDown(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnKeyUp(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnStopMove(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnRotate(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecuteSkill_Q(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecuteSkill_Q_Float3(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecuteSkill_E(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecuteSkill_E_Float3(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnInputQ(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnInputE(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnMouseInput(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecutePowerAttack(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnExecuteCommonAttack(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnStageChangeBoss(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnForceGameEnd(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnGameEndOk(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
  static void OnTimeSyncReq(sh::IO_Engine::TCP_ISessionPtr sessionPtr, BYTE* message);
};
}  // namespace DreamWorld
