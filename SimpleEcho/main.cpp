#include <pch.h>
#include "EchoServer/EchoServer.h"

int main() {
  sh::EchoServer::EchoServer echoServer(4);
  echoServer.Start();

  return 0;
}
