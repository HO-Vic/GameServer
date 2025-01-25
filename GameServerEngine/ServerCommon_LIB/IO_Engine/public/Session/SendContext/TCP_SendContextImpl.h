#pragma once
#include "./ISendContextImpl.h"

namespace sh::IO_Engine {
class SendBuffer;
class TCP_SendContextImpl final
    : public ISendContextImpl {
 public:
  TCP_SendContextImpl(SOCKET sock)
      : ISendContextImpl(sock) {
  }

  virtual void DoSend(const BYTE* data, const size_t len) override;

  virtual void SendComplete(const size_t ioByte) override;

 private:
  virtual void SendExecute() override;

 private:
  std::vector<std::shared_ptr<SendBuffer>> m_sendBuffer;  // Send Completion�� �� �������� �����Ͱ� �־�� ��
  std::queue<std::shared_ptr<SendBuffer>> m_sendQueue;
  std::mutex m_queueLock;
  std::atomic_bool m_isSendAble;
};
}  // namespace sh::IO_Engine
