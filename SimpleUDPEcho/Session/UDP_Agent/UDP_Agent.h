#pragma once
#include <IO_Engine/Session/UDP_IAgent.h>
namespace sh::UDP_Echo {

class UDP_Agent
    : public IO_Engine::UDP_IAgent {
 public:
  UDP_Agent() = default;

  UDP_Agent(SOCKET sock, uint32_t receiverNo, [[maybe_unused]] uint16_t port = 9000);

  virtual ~UDP_Agent();

  virtual void OnDestroy() override;

  virtual void ReleaseSocket(SOCKET socket) override;

 protected:
};
}  // namespace sh::UDP_Echo