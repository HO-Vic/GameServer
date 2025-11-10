#pragma once
#include <cstdint>

namespace sh ::Utility {
class ThWorkerJob;
}  // namespace sh::Utility

namespace sh::IO_Engine {
class TCP_RecvContext {
 public:
  TCP_RecvContext()
      : m_socket(NULL), m_buffer(""), m_remainLen(0), m_recvHandler(nullptr), m_head(0) {
    m_wsaBufs[0].buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBufs[0].len = m_remainLen;
    m_wsaBufs[1].buf = reinterpret_cast<char*>(m_buffer + MAX_RECV_BUF_SIZE);
    m_wsaBufs[1].len = 0;
  }

  TCP_RecvContext(SOCKET sock, TCP_RecvHandler&& TCP_RecvHandler)
      : m_socket(sock), m_buffer(""), m_remainLen(0), m_recvHandler(std::move(TCP_RecvHandler)), m_head(0) {
    m_wsaBufs[0].buf = reinterpret_cast<char*>(m_buffer);
    m_wsaBufs[0].len = m_remainLen;
    m_wsaBufs[1].buf = reinterpret_cast<char*>(m_buffer + MAX_RECV_BUF_SIZE);
    m_wsaBufs[1].len = 0;
  }

  void DefferedSet(SOCKET sock, TCP_RecvHandler&& TCP_RecvHandler) {
    m_socket = sock;
    m_recvHandler = std::move(TCP_RecvHandler);
  }

  //[w][][][][][]...[\w] => 0~1399까지 버퍼 w + 1400~ 0바이트 recv인데
  // 1400~ << 이거는 invalid인데 괜찮은지 테스트, 차라리 nullptr이 나려나 생각

  //[w][][][][][]...[\w]
  //
  //[r][][][\r][w][][][][][\w] //recv돼서 r~\r까지 수신 됨
  //[w2][\w2][st][en][w1][][][][][\w1]//w1~부터 w2~\w2까지 수신 시작
  //
  //[r2][][\r2][st][en][\r1][][][][r1] //recv comple
  //[r2][][\r2][st][][][][][][en]
  //[r2][][\r2][][][][][st ][][en]
  //
  //[][][\r][w][][][][\w][r][] //recv complete
  //[][][\w2][w1][][][][\w1][w2][] // recv start
  //...

  int32_t RecvComplete(Utility::ThWorkerJob* workerJob, DWORD ioSize);

  int32_t StartRecv(Utility::ThWorkerJob* workerJob);

 private:
  int32_t InternalDoRecv(Utility::ThWorkerJob* workerJob, char* buf1, uint32_t buf1Len, char* buf2, uint32_t buf2Len, DWORD bufferCount = 2);

 private:
#ifdef _DEBUG
  struct DebugInfo {
    uint32_t ioSize = 0;
    uint32_t processedSize;
    uint32_t startHead;
    uint32_t endHead;
    DebugInfo() = default;
    DebugInfo(uint32_t io)
        : ioSize(io), processedSize(0), startHead(0), endHead(0) {
    }
    DebugInfo(uint32_t p, uint32_t s, uint32_t e)
        : ioSize(0), processedSize(p), startHead(s), endHead(e) {
    }
  };
  std::vector<DebugInfo> debugLogger;
#endif
  SOCKET m_socket;
  TCP_RecvHandler m_recvHandler;
  WSABUF m_wsaBufs[2];
  BYTE m_buffer[MAX_RECV_BUF_SIZE];
  uint32_t m_head = 0;       // 읽기 시작 위치
  uint32_t m_remainLen = 0;  // 남은 이전 버퍼 정보
};
}  // namespace sh::IO_Engine
