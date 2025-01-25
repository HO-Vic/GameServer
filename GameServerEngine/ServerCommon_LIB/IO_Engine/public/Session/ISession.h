#pragma once
#include <pch.h>

namespace sh {
namespace IO_Engine {
class SessionInpl;
class ISession {
 public:
  void DoSend();

  void DoRecv();

  virtual void OnRecv() = 0;  // ÄÁÅÙÃ÷ ±¸Çö

  void OnSend();

 private:
  SessionInpl* m_sessionImpl;
};
}
}  // namespace sh::IO_Engine
