#include "stdafx.h"
#include <Windows.h>
#include <memory>
#include <chrono>
#include <functional>
#include "RoomMsgDispatcher.h"
#include "../Server/MsgProtocol.h"
#include "../Network/Session/Session.h"
#include "../Room/RoomBase.h"
#include "../Room/Room.h"
#include "../GameObject/Character/ChracterObject.h"

namespace DreamWorld {
void RoomMsgDispatcher::Init() {
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MOVE_KEY_DOWN), std::bind(RoomMsgDispatcher::OnKeyDown, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MOVE_KEY_UP), std::bind(RoomMsgDispatcher::OnKeyUp, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::STOP), std::bind(RoomMsgDispatcher::OnStopMove, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::ROTATE), std::bind(RoomMsgDispatcher::OnRotate, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_EXECUTE_Q), std::bind(RoomMsgDispatcher::OnExecuteSkill_Q, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_Q), std::bind(RoomMsgDispatcher::OnExecuteSkill_Q_Float3, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_EXECUTE_E), std::bind(RoomMsgDispatcher::OnExecuteSkill_E, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_FLOAT3_EXECUTE_E), std::bind(RoomMsgDispatcher::OnExecuteSkill_E_Float3, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_INPUT_Q), std::bind(RoomMsgDispatcher::OnInputQ, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::SKILL_INPUT_E), std::bind(RoomMsgDispatcher::OnInputE, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::MOUSE_INPUT), std::bind(RoomMsgDispatcher::OnMouseInput, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::PLAYER_POWER_ATTACK_EXECUTE), std::bind(RoomMsgDispatcher::OnExecutePowerAttack, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::PLAYER_COMMON_ATTACK_EXECUTE), std::bind(RoomMsgDispatcher::OnExecuteCommonAttack, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::GAME_END_OK), std::bind(RoomMsgDispatcher::OnGameEndOk, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::TIME_SYNC_REQUEST), std::bind(RoomMsgDispatcher::OnTimeSyncReq, std::placeholders::_1, std::placeholders::_2));

#pragma region CHEAT
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::STAGE_CHANGE_BOSS), std::bind(RoomMsgDispatcher::OnStageChangeBoss, std::placeholders::_1, std::placeholders::_2));
  AddMsgHandler(static_cast<uint8_t>(DreamWorld::CLIENT_PACKET::TYPE::TEST_GAME_END), std::bind(RoomMsgDispatcher::OnForceGameEnd, std::placeholders::_1, std::placeholders::_2));
#pragma endregion
}

void RoomMsgDispatcher::OnKeyDown(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::MovePacket*>(message);

  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();

  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }

  possessObject->RecvDirection(recvPacket->direction, true);
  DreamWorld::SERVER_PACKET::MovePacket sendPacket(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(DreamWorld::SERVER_PACKET::TYPE::MOVE_KEY_DOWN));
  roomRef->Broadcast(reinterpret_cast<DreamWorld::PacketHeader*>(&sendPacket), userSession);
}

void RoomMsgDispatcher::OnKeyUp(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::MovePacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::MovePacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();

  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }

  possessObject->RecvDirection(recvPacket->direction, false);

  DreamWorld::SERVER_PACKET::MovePacket sendPacket(possessObject->GetRole(), recvPacket->direction, std::chrono::high_resolution_clock::now(), static_cast<char>(DreamWorld::SERVER_PACKET::TYPE::MOVE_KEY_UP));
  roomRef->Broadcast(reinterpret_cast<DreamWorld::PacketHeader*>(&sendPacket), userSession);
}

void RoomMsgDispatcher::OnStopMove(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::StopPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::StopPacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();

  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->StopMove();

  auto sendPacket = DreamWorld::SERVER_PACKET::StopPacket(possessObject->GetRole(), std::chrono::high_resolution_clock::now(), static_cast<char>(DreamWorld::SERVER_PACKET::TYPE::STOP));
  roomRef->Broadcast(&sendPacket, userSession);
}

void RoomMsgDispatcher::OnRotate(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::RotatePacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::RotatePacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvRotate(recvPacket->axis, recvPacket->angle);
  auto sendPacket = DreamWorld::SERVER_PACKET::RotatePacket(possessObject->GetRole(), recvPacket->axis, recvPacket->angle);
  roomRef->Broadcast(&sendPacket, userSession);
}

void RoomMsgDispatcher::OnExecuteSkill_Q(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }

  possessObject->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q);
}

void RoomMsgDispatcher::OnExecuteSkill_Q_Float3(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  const DreamWorld::CLIENT_PACKET::FloatDataSkillPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::FloatDataSkillPacket*>(message);
  possessObject->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q, recvPacket->floatData);
}

void RoomMsgDispatcher::OnExecuteSkill_E(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_E);
}

void RoomMsgDispatcher::OnExecuteSkill_E_Float3(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  const DreamWorld::CLIENT_PACKET::FloatDataSkillPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::FloatDataSkillPacket*>(message);
  possessObject->RecvSkill(CharacterObject::SKILL_TYPE::SKILL_TYPE_E, recvPacket->floatData);
}

void RoomMsgDispatcher::OnInputQ(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvSkillInput(CharacterObject::SKILL_TYPE::SKILL_TYPE_Q);
}

void RoomMsgDispatcher::OnInputE(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvSkillInput(CharacterObject::SKILL_TYPE::SKILL_TYPE_E);
}

void RoomMsgDispatcher::OnMouseInput(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::MouseInputPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::MouseInputPacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  // Room객체가 아직 유효
  possessObject->RecvMouseInput(recvPacket->leftClickedButton, recvPacket->rightClickedButton);
  auto sendPacket = DreamWorld::SERVER_PACKET::MouseInputPacket(possessObject->GetRole(), recvPacket->leftClickedButton, recvPacket->rightClickedButton);
  roomRef->Broadcast(&sendPacket, userSession);
}

void RoomMsgDispatcher::OnExecutePowerAttack(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::PlayerPowerAttackPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::PlayerPowerAttackPacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvAttackCommon(recvPacket->direction, recvPacket->power);
}

void RoomMsgDispatcher::OnExecuteCommonAttack(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  const DreamWorld::CLIENT_PACKET::PlayerCommonAttackPacket* recvPacket = reinterpret_cast<const DreamWorld::CLIENT_PACKET::PlayerCommonAttackPacket*>(message);
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  possessObject->RecvAttackCommon(recvPacket->direction);
}

void RoomMsgDispatcher::OnStageChangeBoss(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  // auto changeBossStageEvent = std::make_shared<ChangeBossStageEvent>(roomRef);
  // roomRef->InsertPrevUpdateEvent(std::static_pointer_cast<PrevUpdateEvent>(changeBossStageEvent));
}

void RoomMsgDispatcher::OnForceGameEnd(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  // roomRef->ForceGameEnd();
}

void RoomMsgDispatcher::OnGameEndOk(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  auto userSession = std::static_pointer_cast<DreamWorld::Session>(sessionPtr);
  auto possessObject = userSession->GetPossessCharacter();
  auto roomRef = userSession->GetRoom();
  if (nullptr == possessObject || nullptr == roomRef) {
    return;  // Already Expired Room
  }
  roomRef->DiscardPlayer(userSession);
  //  m_playerState.store(PLAYER_STATE::LOBBY);
}

void RoomMsgDispatcher::OnTimeSyncReq(sh::IO_Engine::ISessionPtr sessionPtr, BYTE* message) {
  DreamWorld::SERVER_PACKET::TimeSyncPacket sendPacket{};
  sessionPtr->DoSend(&sendPacket, sendPacket.size);
}

}  // namespace DreamWorld