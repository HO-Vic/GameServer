#pragma once
#include <PCH/pch.h>
/*
        �Լ��ڸ� ���� Ŭ���� ��ť�� ���� ����
        �����ڷ�, T, function Args�� �ޱ�
*/

namespace sh::Utility {

class JobPoolBase;

class Job : public std::enable_shared_from_this<Job> {
  using Caller = std::function<void()>;

 public:
  Job();
  Job(Caller&& caller);

  void Set(Caller&& caller);

  void Execute() {
    if (nullptr != m_job) {
      m_job();
      m_job = nullptr;
    }
  }

 private:
  Caller m_job;

 public:
  // �������� �ñ�������
  // �� ���� ĸ��
  // "[=]" : ����
  // "[&]" : ����
  // �Ʒ��� shared_ptr�� �����̱� ������, ������ ����
  template <typename T, typename... Args>
  static Caller GenerateCaller(std::function<void(Args...)> func, std::shared_ptr<T>& owner, Args... args) {
    if (nullptr == owner) {
      return [=]() { func(std::forward<Args>(args)...); };
    }
    return [=]() { owner->func(std::forward<Args>(args)...); };
  }
};
}  // namespace sh::Utility
