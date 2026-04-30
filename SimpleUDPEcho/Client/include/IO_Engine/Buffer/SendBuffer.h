#pragma once
#include <cassert>
#include <cstdint>
#include <memory>
#include <Winsock2.h>
#include <ws2def.h>
#include "Utility/Thread/IWorkerItem.h"
#include "../CommonDefine.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {

constexpr static uint32_t MAX_SEND_BUFFER_SIZE = 1024;
class UDP_AgentBase;
class UDP_SessionBase;

// 128, 512, 1024, 2048
// 버퍼 풀링해도 ~vector() 호출되어서 heap 재할당되니
// template로 size 받고 풀링
// 일정 크기 이상은 동적 heap 할당으로

// 가상 함수보다는 ptr, len, cap을 Base클래스에 두고
// 자식 클래스 할당된 메모리의 ptr을 생성자에서 꽂아주는 방식으로 하는게 가상 함수 오버헤드 없애고 더 좋다 생각
// class SendBufferBase {
//  public:
//   virtual WSABUF GetBuffer() = 0;
//   virtual void Write(uint32_t offset, uint32_t len) = 0;
// };

class SendBufferBase {
  static constexpr uint32_t HEADER_SIZE = sizeof(PacketHeader);

 public:
  SendBufferBase() = default;
  SendBufferBase(BYTE* dataPtr, uint32_t capSize)  // 헤더 크기를 미리 썼다고 가정
      : data(dataPtr), cap(capSize), writeSize(HEADER_SIZE) {
  }
  ~SendBufferBase() = default;

  WSABUF GetWSABuffer() const {
    return {writeSize, reinterpret_cast<char*>(data)};
  }

  BYTE* GetWritePtr(uint32_t len);

  void Write(const BYTE* srcPtr, uint32_t len);

  uint32_t GetWriteAbleSize() const {
#ifdef _DEBUG
    assert(writeSize > cap);
#endif  // _DEBUG
    return cap - writeSize;
  }

 protected:
  BYTE* data = nullptr;  // 상속 받은 객체가 반환할거기때문에, 절대로 해제xx
  uint32_t writeSize = HEADER_SIZE;
  uint32_t cap = 0;
};

template <uint32_t SIZE>
class TCP_SendBuffer final
    : public SendBufferBase {
 public:
  TCP_SendBuffer()
      : SendBufferBase(buffer, SIZE) {
  }

 private:
  BYTE buffer[SIZE] = {0};
};

class DynamicSendBuffer final
    : public SendBufferBase {
 public:
  DynamicSendBuffer(uint32_t len)
      : SendBufferBase(), buffer{std::make_unique<BYTE[]>(len)} {
    // 생성자 멤버 이니셜 라이즈 리스트에서 buffer{make_unique}, SendBufferBase(buffer.get(), len) 하더라도
    // 멤버 필드 순서대로 초기화 되기 때문에, SendBufferBase(nullptr, len), buffer(make_unique)가 됩니다.
    // 그래서 생성자 내부에서 처리
    data = buffer.get();
    cap = len;
  }

 private:
  std::unique_ptr<BYTE[]> buffer;
};
//
class SendBuffer {
 public:
  SendBuffer() = default;
  SendBuffer(const BYTE* data, const uint32_t len);

 public:
  uint32_t m_size;
  BYTE m_buffer[MAX_SEND_BUFFER_SIZE];
};

class UDP_SingleSendBuffer final
    : public Utility::IWorkerItem,
      public SendBuffer {
 public:
  UDP_SingleSendBuffer() = default;

  UDP_SingleSendBuffer(std::shared_ptr<UDP_AgentBase>& agentPtr, std::shared_ptr<UDP_SessionBase>& sessionPtr, const BYTE* data, const uint32_t len);

  virtual bool Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const DWORD errorCode) override;

  uint32_t DoSend(SOCKET sock, sockaddr_in toAddr);

 private:
  WSABUF m_wsaBuf;
  std::weak_ptr<UDP_AgentBase> m_agentPtr;
  std::weak_ptr<UDP_SessionBase> m_sessionPtr;
  uint32_t m_retransmitCnt = 0;
};

}  // namespace sh::IO_Engine