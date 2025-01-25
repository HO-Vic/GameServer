#pragma once
#include <pch.h>
// #include <Utility/SpinLock/SpinLock.h>

/*
        UDP�� ��Ŷ ���� �����°� �ǹ� ���°ɷ� ����.
        �׳� Send()�� ��, Send()�ϴ°� �¾ƺ���.
        ##
        TCP�� strean�̴ϱ� �̾� �ٿ��� �ѹ��� ��ġ�°� �����ɷ� ����
                �� ��ġ�°� ��� �Ұ���?
                         �� 1. �׼��� �����ο����� shared_ptr<>(Ǯ���� ������
   ����)�� ���ǵ�, ���⵵ shared_ptr�� �ϴ°� �ƴ϶� ����. �� 2. �ش� ���� ���
   �Ҵ��ϸ� heap �޸� ����ȭ ����
*/

namespace sh::IO_Engine {
class ISendContextImpl;
enum SEND_TYPE : char {
  TCP = 1,
  UDP = 2
};

class SendContext {
 public:
  SendContext(SOCKET sock, const SEND_TYPE sendType);

  ~SendContext();

  void DoSend(const BYTE* sendPacket, const size_t len);

  void SendComplete(const size_t ioByte);

 private:
  ISendContextImpl* m_sendContextImpl;
};
}  // namespace sh::IO_Engine