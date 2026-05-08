#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
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
  class InternalDoubleBufferQueue {
   public:
    InternalDoubleBufferQueue()
        : m_activeIdx(false) {
    }

    void InsertSendBuffer(std::shared_ptr<SendBufferBase>&&);

    void InsertBatch(std::vector<std::shared_ptr<SendBufferBase>>&&);

    std::vector<std::shared_ptr<SendBufferBase>>& SwapAndLoad();

   private:
    std::vector<std::shared_ptr<SendBufferBase>> m_sendQueues[2];
    std::mutex m_lock;
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

  // 송신 트리거. CAS 단일 송신권 보장. 멀티스레드/중복 호출 무해.
  int32_t Flush(Utility::WorkerPtr session);

  int32_t SendComplete(Utility::ThWorkerJob* workerJob, const size_t ioByte);

 private:
  // CAS 시도 + SendExecute 호출. DoSend(Immediate) / Flush()의 공통 경로.
  int32_t TryFlush(Utility::WorkerPtr session);

  // m_sendBuffer와 batchSendBuffers와 swap해야해서 참조형으로
  int32_t SendExecute(Utility::ThWorkerJob* workerJob, std::vector<std::shared_ptr<SendBufferBase>>& batchSendBuffers);

 private:
  std::vector<std::shared_ptr<SendBufferBase>> m_sendBuffer;  // Send Completion이 올 때까지는 데이터가 있어야 함
  InternalDoubleBufferQueue m_doubleQueue;
  std::vector<WSABUF> m_wsaBuffers;
  // Concurrency::concurrent_queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  SOCKET m_socket;
  std::atomic_bool m_isSendAble;
};
}  // namespace sh::IO_Engine
