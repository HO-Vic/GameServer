#pragma once
#include <pch.h>
// #include <Utility/SpinLock/SpinLock.h>

/*
        UDP도 패킷 뭉쳐 보내는게 의미 없는걸로 보임.
        그냥 Send()할 떄, Send()하는게 맞아보임.
        ##
        TCP는 strean이니까 이어 붙여서 한번에 뭉치는게 맞을걸로 보임
                ㄴ 뭉치는걸 어떻게 할건지?
                         ㄴ 1. 겜서버 구현부에서는 shared_ptr<>(풀링된 프로토
   버퍼)를 쓸건데, 여기도 shared_ptr로 하는건 아니라 생각. ㄴ 2. 해당 버퍼 계속
   할당하면 heap 메모리 파편화 생각
*/

namespace sh::IO_Engine {
class ISendContextImpl;
class OverlappedEx;
class IOverlappedEvent;
using OverlappedPtr = std::shared_ptr<IOverlappedEvent>;
class SendContext {
 public:
  SendContext(SOCKET sock, const IO_TYPE sendType);

  ~SendContext();

  void DoSend(OverlappedPtr& session, const BYTE* sendPacket, const size_t len);

  void SendComplete(OverlappedEx* overlappedEx, const size_t ioByte);

 private:
  ISendContextImpl* m_sendContextImpl;
};
}  // namespace sh::IO_Engine