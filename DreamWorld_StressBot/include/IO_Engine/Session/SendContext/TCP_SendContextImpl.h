#pragma once
#include "./ISendContextImpl.h"
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>

namespace sh::IO_Engine {
class SendBuffer;
class OverlappedEx;
class TCP_SendContextImpl final
    : public ISendContextImpl {
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
  TCP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock), m_isSendAble(true) {
  }

  virtual int32_t DoSend(Utility::WorkerPtr& session, const BYTE* data, const size_t len) override;

  virtual int32_t SendComplete(Utility::ThWorkerJob* thWorkerJob, const size_t ioByte) override;

 private:
  virtual int32_t SendExecute(Utility::ThWorkerJob* thWorkerJob) override;

 private:
  std::vector<std::shared_ptr<SendBuffer>> m_sendBuffer;  // Send Completion이 올 때까지는 데이터가 있어야 함
  std::queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  std::mutex m_queueLock;
  std::atomic_bool m_isSendAble;
};
}  // namespace sh::IO_Engine
