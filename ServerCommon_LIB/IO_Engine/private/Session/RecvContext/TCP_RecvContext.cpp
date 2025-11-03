#pragma once
#include "pch.h"
#include <Session/RecvContext/TCP_RecvContext.h>
#include <Session/TCP_ISession.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Thread/ThWorkerJob.h>
#include <IO_Core/ThWorkerJobPool.h>

namespace sh::IO_Engine {
int32_t TCP_RecvContext::RecvComplete(Utility::ThWorkerJob* workerJob, DWORD ioSize) {
#ifdef _DEBUG
  debugLogger.emplace_back(ioSize);
#endif  // _DEBUG

  uint32_t remainSize = ioSize + m_remainLen;
  auto head = m_head;

  while (remainSize > sizeof(PacketHeader::size)) {
    //[][][][][en][][st] 인 경우도 생각해야
    // 1바이트 끝에 걸린채로 나머지 바이트가 존재하는 경우
    if (head >= MAX_RECV_BUF_SIZE) {
      head = head % MAX_RECV_BUF_SIZE;
    }
    // 현재 읽는 위치가 버퍼 끝부분인 경우(끝부분 1바이트~)
    if (head == MAX_RECV_BUF_SIZE - 1) {
      /*
       이 if는 패킷헤더에 deserialize 하지 않음, 잘린 패킷헤더에 대해서 통신간에 더 큰수,작은수 에 대한 바이트 오더만 구분 필요했음
       패킷헤더의 바이트 오더를 빅 엔디안으로 통일
       큰 값은 낮은 주소에 저장됨 -> head << 8 | m_buffer
      */
      PacketHeader currentPacket(&m_buffer[head], m_buffer);  // 내부적으로 비트 시프트해서 size를 저장
      /*
        ex)
        m_buffer = 0
        MAX_RECV_BUF_SIZE = 1400
        bufferPosition = 1399
        1399 >= 0+1400-1 => true (끝에 1byte)
        1byte memcpy
        size-1 byte memcpy
        process Packet
        bufferPosition = 0+size-1; 패킷의 사이즈가 size지만, 마지막 1byte까지 썼기때문에, size-1 위치로
      */
      BYTE mergeBuffer[MAX_RECV_BUF_SIZE] = {0};  // heap할당보다는 스택에서 링버퍼 처리할 임시 버퍼
      // mergeBuffer에 패킷에 해당하는 만큼 복사하여 처리

      // memcpy(mergeBuffer, &m_buffer[head], 1);
      memcpy(mergeBuffer, m_buffer + 1, static_cast<uint64_t>(currentPacket.size - sizeof(PacketHeader)));

      m_recvHandler(std::static_pointer_cast<TCP_ISession>(workerJob->GetWorkerItem()), currentPacket.size, mergeBuffer);
#ifdef _DEBUG
      auto prevHead = head;
#endif  // _DEBUG

      remainSize -= currentPacket.size;
      head = currentPacket.size - 1;

#ifdef _DEBUG
      debugLogger.emplace_back(currentPacket.size, prevHead, head);
#endif  // _DEBUG
      continue;
    }

    PacketHeader* currentPacket = reinterpret_cast<PacketHeader*>(&m_buffer[head]);
    currentPacket->Deserialize();
    if (currentPacket->size > remainSize) {
      // 완성된 패킷이 만들어지지 않음.
      break;
    }

    // 패킷 크기가 선형 버퍼 크기를 벗어나는 경우
    // 패킷의 크기를 알 수 있지만
    // [][][en][][][st][][] << 이런 경우일 수 있음
    // 위의 경우인지 판단하여, 위와 같이 잘린 버퍼를 임시 버퍼에 선형버퍼로 복사하여 처리
    if (head + currentPacket->size > MAX_RECV_BUF_SIZE - 1) {
      /*
        ex)
        m_buffer = 0
        MAX_RECV_BUF_SIZE = 1400
        bufferPosition = 1380
        packetSize = 22
        1380+22=1402 > 0+1400-1 : true
        frontSize = 1400 -(1380-0) = 20 (1380~1399까지 메모리)
        backSize = packetSize - frontSize; = 2
        process Packet
        buferrPosition = m_buffer+backSize;
      */
      int64_t frontSize = static_cast<int64_t>(MAX_RECV_BUF_SIZE - head);  // 패킷의 앞부분 크기
      int64_t backSize = currentPacket->size - frontSize;                  // 패킷의 뒷부분 크기

      BYTE mergeBuffer[MAX_RECV_BUF_SIZE] = {0};                // heap할당보다는 스택에서 링버퍼 처리할 임시 버퍼
      memcpy(mergeBuffer, &m_buffer[head] + 2, frontSize - 2);  // 버퍼포지션~마지막 버퍼 크기만큼
      memcpy(mergeBuffer + frontSize - 2, &m_buffer[0], backSize);

      m_recvHandler(std::static_pointer_cast<TCP_ISession>(workerJob->GetWorkerItem()), currentPacket->size, mergeBuffer);
#ifdef _DEBUG
      auto prevHead = head;
#endif  // _DEBUG
      remainSize -= currentPacket->size;

      head = static_cast<uint32_t>(backSize);  // 새로 읽을 위치는 선형 버퍼의 앞에서 backSize만큼 뒤

#ifdef _DEBUG
      debugLogger.emplace_back(currentPacket->size, prevHead, head);
#endif  // _DEBUG
      continue;
    }

    //  완성된 패킷이 선형적으로 있어서, m_buffer의 포인터인 bufferPosition으로 패킷 처리
    m_recvHandler(std::static_pointer_cast<TCP_ISession>(workerJob->GetWorkerItem()), currentPacket->size, (&m_buffer[head + 2]));
    // 남은 퍼버 크기 최신화, 현재 버퍼 위치 다음 패킷 시작 위치로
#ifdef _DEBUG
    auto prevHead = head;
#endif  // _DEBUG
    remainSize -= currentPacket->size;
    head = (head + currentPacket->size) % MAX_RECV_BUF_SIZE;
#ifdef _DEBUG
    debugLogger.emplace_back(currentPacket->size, prevHead, head);
#endif  // _DEBUG
  }

  m_remainLen = remainSize;  // 현재 남은 데이터 크기 저장
  m_head = head;             // 현재 읽기 시도 부분이 첫 읽기 위치
  /*
  [1][\1][h][t][0][][][\0] << 이 형태
  [t][][][][][][][h] << 이런 형태라면?
  [t][][][]          [][][][h]
  [t][][][]          [][][][h]
  => x2 + 1 ~ x1-1까지
  if(h <= t) {
  [][][h][t][][]
    t+1~last; // => k
    0~h; // => l
    [l][l][h][t][k][k]
    case 2. head=tail인 케이스
    [][][][][][][][ht]
    t+1~last//k
    0~h-1 // l
    [l][l][l][l][l][l][l][ht]

  } else if(t<h) {
    t+1~h-1 // k
    [t][k][k][...][k][k][h]
  }
  */

  /*
  MAX = 1400;
  m_buffer = 0;
  remain = 5;

  h = 1397;
  t = 1;

  headIdx = 1397
  backSize = 1400 - 1397; = 3

  if remain(5) - 3 > 0: => tail이 head보다 앞에 있는 경우
    tail+1 ~ head-1
  else: remain(<=3) - 3 < 0: 음수 => tail이 head보다 뒤에 존재함(같은 위치 가능)
    tail+1 ~ last, head+1~tail-1 범위를 가짐
  */

  // 남은 데이터 헤드부터 끝까지했을 때, 남은 공간
  //[R2][tail][][][...][][][head][R1][R1] => R1로 표시된 부분의 크기
  int32_t restLinearSize = static_cast<int32_t>(MAX_RECV_BUF_SIZE - head);

  if (static_cast<int32_t>(m_remainLen - restLinearSize) >= 0) {  // 남은 패킷의 tail이 head보다 앞에 있는 경우 ->[0][t]  [2][][...][][1396]  [h][1398][1399]
    // 앞으로 넘어간 패킷의 뒷부분 크기
    uint32_t tail = static_cast<uint32_t>(m_remainLen - restLinearSize);  // 125Line, R2의 크기
                                                                          // 2~1396 => 1395의 크기
                                                                          // (haed-1) - (tail-1) => (1397 -1) - (2-1) = head-tail
    return InternalDoRecv(workerJob, reinterpret_cast<char*>(&m_buffer[tail]), static_cast<uint32_t>(head - tail), nullptr, 0, 1);
    // buffer[tail] => tail의 다음 버퍼
    //  WSARecv에서 wsaBuf[1].buf = nullptr해도 문제 없을까?
    //  ㄴ> dwcount를 1로 조정해서 하면 문제 없음
  }
  // head뒤에 tail이 있는 경우
  // [0][1] [h][~][t] [1397][1398][1399]
  /*
    head: 1390
    m_remain = 7
    t: 1397

    wsabuf[0].len = MAX(1400) - 1397 => 3 ==> 1397 1398 1989

    wsabuf[1].len = head-0 = 1390, 0~1389
  */
  uint32_t tail = head + m_remainLen;  // exclusive
  // if (tail == MAX_RECV_BUF_SIZE) {    // 0~head => 132Line에서 처리
  // }
  return InternalDoRecv(workerJob, reinterpret_cast<char*>(&m_buffer[tail]), static_cast<uint32_t>(MAX_RECV_BUF_SIZE - tail), reinterpret_cast<char*>(&m_buffer[0]), static_cast<uint32_t>(head));
}

int32_t TCP_RecvContext::StartRecv(Utility::ThWorkerJob* workerJob) {
  // 0~1399 + nullptr,0 =>dwCount: 1
  return InternalDoRecv(workerJob, reinterpret_cast<char*>(m_buffer), MAX_RECV_BUF_SIZE, nullptr, 0, 1);
}

// 링버퍼 쓸거면, wsaBuf를 2개 두고 하나는 tail부분으로 하고, 하나는 0~head까지로 하면 됨. << 유레카
int32_t TCP_RecvContext::InternalDoRecv(Utility::ThWorkerJob* workerJob, char* buf1, uint32_t buf1Len, char* buf2, uint32_t buf2Len, DWORD bufferCount /*= 2*/) {
  // wsaBuf의 buf 위치를 바꿈
  m_wsaBufs[0].buf = buf1;
  m_wsaBufs[0].len = buf1Len;
  m_wsaBufs[1].buf = buf2;
  m_wsaBufs[1].len = buf2Len;
  DWORD flag = 0;
  auto errorNo = WSARecv(m_socket, m_wsaBufs, bufferCount, nullptr, &flag, reinterpret_cast<LPOVERLAPPED>(workerJob), nullptr);
  if (0 != errorNo) {
    auto wsaErr = WSAGetLastError();
    if (WSA_IO_PENDING == wsaErr) {
      errorNo = 0;
    } else {
      errorNo = wsaErr;
    }
  }
  return errorNo;
}
}  // namespace sh::IO_Engine
