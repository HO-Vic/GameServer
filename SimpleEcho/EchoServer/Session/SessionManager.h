#pragma once
#include <Utility/Pool/ObjectPool.h>

namespace sh::EchoServer {
class Session;
class SessionManager
    : public Utility::ObjectPool<Session> {
 public:
  SessionManager();
};
}  // namespace sh::EchoServer
