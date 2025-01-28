#pragma once
#include "./ISendContextImpl.h"

namespace sh::IO_Engine {
class SendBuffer;
class OverlappedEx;
class TCP_SendContextImpl final
    : public ISendContextImpl {
 public:
  TCP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock) {
  }

  virtual void DoSend(OverlappedPtr& session,const BYTE* data, const size_t len) override;

  virtual void SendComplete(OverlappedEx* overlappedEx, const size_t ioByte) override;

 private:
  virtual void SendExecute(OverlappedEx* overlappedEx) override;

 private:
  std::vector<std::shared_ptr<SendBuffer>> m_sendBuffer;  // Send Completion이 올 때까지는 데이터가 있어야 함
  std::queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  std::mutex m_queueLock;
  std::atomic_bool m_isSendAble;
};
}  // namespace sh::IO_Engine
