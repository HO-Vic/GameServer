#include "pch.h"
#include "NetworkModule.h"
#include "../../DreamWorld_Server/Server/MsgProtocol.h"

namespace Stress {
void NetworkModule::InitMsgDispatcher() {
  DreamWorld::SERVER_PACKET::TYPE::LOGIN_SUCCESS;
  DreamWorld::SERVER_PACKET::TYPE::INTO_GAME;
  DreamWorld::SERVER_PACKET::TYPE::GAME_STATE_STAGE;
  DreamWorld::SERVER_PACKET::TYPE::GAME_STATE_BOSS;
  DreamWorld::SERVER_PACKET::TYPE::GAME_END;
  DreamWorld::SERVER_PACKET::TYPE::STRESS_TEST_DELAY;
}

void NetworkModule::OnLoginSuccess(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  // auto currentTIme = TIME::now();
  // auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTIme - m_loginSendTime);
  // long long currentDelayTime = durationTime.count();  // currentDelay

  // long long diffPrevDelay = currentDelayTime - m_delayTime;  // 이전 딜레이와 현재 딜레이 차이

  // auto prevGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
  ///*if (prevGlobalDelay > 2021090) {
  //        std::cout << "asf" << std::endl;
  //}*/
  // DreamWorld::StressTestNetwork::GetInstance().globalDelay += diffPrevDelay;
  // auto debugGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
  ///*if (debugGlobalDelay > 2021090) {
  //        std::cout << "asf" << std::endl;
  //}*/
  // m_delayTime = currentDelayTime;

  // DreamWorld::StressTestNetwork::GetInstance().IncreaseActiveClient();
  // int activeNum = DreamWorld::StressTestNetwork::GetInstance().GetActiveNum();
  // double averValue = 0.0f;
  // averValue = (double)currentDelayTime / (double)activeNum;
  // DreamWorld::StressTestNetwork::GetInstance().dGlobalDelay += (averValue - m_dDelayTime);
  // m_dDelayTime = averValue;
  // m_isActive = true;
}

void NetworkModule::OnIntoInGame(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  /*const SERVER_PACKET::IntoGamePacket* recvPacket = reinterpret_cast<const SERVER_PACKET::IntoGamePacket*>(executePacketHeader);
  m_currentRole = recvPacket->role;*/
}

void NetworkModule::OnGameState_Stage(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  // const SERVER_PACKET::GameState_STAGE* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_STAGE*>(executePacketHeader);
  // for (int i = 0; i < 4; ++i) {
  //   if (recvPacket->userState[i].role == m_currentRole) {
  //     m_x = recvPacket->userState[i].position.x;
  //     m_z = recvPacket->userState[i].position.z;
  //     break;
  //   }
  // }
}

void NetworkModule::OnGameState_Boss(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  /*const SERVER_PACKET::GameState_BOSS* recvPacket = reinterpret_cast<const SERVER_PACKET::GameState_BOSS*>(executePacketHeader);
  for (int i = 0; i < 4; ++i) {
    if (recvPacket->userState[i].role == m_currentRole) {
      m_x = recvPacket->userState[i].position.x;
      m_z = recvPacket->userState[i].position.z;
      break;
    }
  }*/
}

void NetworkModule::OnGameEnd(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  // 상태 변경
}

void NetworkModule::OnStressTestDelay(sh::IO_Engine::ISessionPtr session, size_t, BYTE* packetHeader) {
  // static std::atomic_llong MIN_DELAY = 100;
  //// 여기서 딜레이 파악
  // auto nowTime = Time::now();
  //// 현재 시간과 보낸 시간의 차이를 계산
  // auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_lastDelaySendTime);
  // long long currentDelayTime = durationTime.count();  // currentDelay

  // if (StressTestNetwork::GetInstance().globalMaxDelay < currentDelayTime) {
  //   StressTestNetwork::GetInstance().globalMaxDelay = currentDelayTime;
  // }

  // if (MIN_DELAY < currentDelayTime) {
  //   MIN_DELAY = currentDelayTime;
  //   std::cout << currentDelayTime << std::endl;
  // }

  // long long diffPrevDelay = currentDelayTime - m_delayTime;  // 이전 딜레이와 현재 딜레이 차이
  //// 빨라졌다면 globalDelay가 줄어들 것.
  // auto prevGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
  ///*if (prevGlobalDelay > 2021090) {
  //        std::cout << "asf" << std::endl;
  //}*/
  // DreamWorld::StressTestNetwork::GetInstance().globalDelay += diffPrevDelay;
  // auto debugGlobalDelay = DreamWorld::StressTestNetwork::GetInstance().globalDelay.load();
  ///*if (debugGlobalDelay > 2021090) {
  //        std::cout << "asf" << std::endl;
  //}*/
  // m_delayTime = currentDelayTime;

  // int activeNum = DreamWorld::StressTestNetwork::GetInstance().GetActiveNum();
  // double averValue = (double)currentDelayTime / (double)activeNum;
  // DreamWorld::StressTestNetwork::GetInstance().dGlobalDelay += (averValue - m_dDelayTime);
  // m_dDelayTime = averValue;
  // m_isAbleCheckDelay = true;
}
}  // namespace Stress