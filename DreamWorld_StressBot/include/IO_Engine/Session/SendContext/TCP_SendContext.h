#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <tbb/concurrent_queue.h>

namespace sh {
namespace Utility {
class ThWorkerJob;
class IWorkerItem;
using WorkerPtr = std::shared_ptr<IWorkerItem>;
}
}  // namespace sh::Utility

namespace sh::IO_Engine {
class SendBuffer;
class OverlappedEx;
class TCP_SendContext final {
  class InternalDoubleBufferQueue {
   public:
    InternalDoubleBufferQueue()
        : m_activeIdx(false) {
    }

    void InsertSendBuffer(std::shared_ptr<SendBuffer>&&);

    std::queue<std::shared_ptr<SendBuffer>>& SwapAndLoad();

   private:
    std::queue<std::shared_ptr<SendBuffer>> m_sendQueues[2];
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

  int32_t DoSend(Utility::WorkerPtr session, const BYTE* data, const size_t len);

  int32_t SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte);

 private:
  int32_t SendExecute(Utility::ThWorkerJob* thWorkerJob);

 private:
  std::vector<std::shared_ptr<SendBuffer>> m_sendBuffer;  // Send Completion이 올 때까지는 데이터가 있어야 함
  tbb::concurrent_queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  SOCKET m_socket;
  std::atomic_bool m_isSendAble;
};
}  // namespace sh::IO_Engine
