#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <concurrent_queue.h>

namespace sh {
namespace Utility {
class ThWorkerJob;
class IWorkerItem;
using WorkerPtr = std::shared_ptr<IWorkerItem>;
}
}  // namespace sh::Utility

namespace sh::IO_Engine {
enum class SendPolicy {
  Immediate,  // 적재 후 즉시 Flush 시도 (기존 동작)
  Deferred,   // 적재만. Flush 시도 없음. 호출자가 Flush() 또는 PushBatch+Flush로 트리거.
};

class SendBufferBase;
class OverlappedEx;
class TCP_SendContext final {
  // 동기화 책임 없는 순수 자료구조. 외부 m_lock으로 보호된다.
  // 책임 분리:
  //   - Insert/SwapBuffer: active 인덱스나 active 큐를 mutate → caller가 lock
  //   - Consume: inactive 큐 ref 반환. SwapBuffer 후 그리고 다음 SwapBuffer 전까지 안정 → lock 없이 호출 가능
  class InternalDoubleBufferQueue {
   public:
    InternalDoubleBufferQueue()
        : m_activeIdx(false) {
    }

    void InsertSendBuffer(std::shared_ptr<SendBufferBase>&&);

    void InsertBatch(std::vector<std::shared_ptr<SendBufferBase>>&&);

    // active idx flip. caller가 m_lock 보유.
    void SwapBuffer();

    // inactive(소비 대상) 큐 참조. SwapBuffer 이후/다음 SwapBuffer 이전까지 안정.
    // claim 보유 중이면 lock 없이 호출 가능.
    std::vector<std::shared_ptr<SendBufferBase>>& Consume();

   private:
    std::vector<std::shared_ptr<SendBufferBase>> m_sendQueues[2];
    bool m_activeIdx;
  };

 public:
  TCP_SendContext()
      : m_socket(NULL), m_isSendAble(true) {
  }

  TCP_SendContext(SOCKET sock)
      : m_socket(sock), m_isSendAble(true) {
  }

  void DeferredSet(SOCKET sock) {
    m_socket = sock;
  }

  int32_t DoSend(Utility::WorkerPtr session, std::shared_ptr<SendBufferBase>&& buffer, SendPolicy policy = SendPolicy::Immediate);

  // 일괄 적재. mutex 1회로 vector 전체 push. Flush 자동 호출 안 함.
  void PushBatch(std::vector<std::shared_ptr<SendBufferBase>>&& buffers);

  // 송신 트리거. m_lock 안에서 송신권 claim. 멀티스레드/중복 호출 무해.
  int32_t Flush(Utility::WorkerPtr session);

  int32_t SendComplete(Utility::ThWorkerJob* workerJob, const size_t ioByte);

 private:
  // m_sendBuffer와 batch를 swap 후 WSASend. lock 밖에서 호출.
  int32_t SendExecute(Utility::ThWorkerJob* workerJob, std::vector<std::shared_ptr<SendBufferBase>>& batch);

 private:
  std::vector<std::shared_ptr<SendBufferBase>> m_sendBuffer;  // Send Completion이 올 때까지는 데이터가 있어야 함
  InternalDoubleBufferQueue m_doubleQueue;
  std::vector<WSABUF> m_wsaBuffers;
  std::mutex m_lock;  // m_doubleQueue + m_isSendAble 동시 보호
  // Concurrency::concurrent_queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  SOCKET m_socket;
  bool m_isSendAble;
};
}  // namespace sh::IO_Engine
