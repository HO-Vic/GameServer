#pragma once
#include <pch.h>

/*
        �Լ��ڸ� ���� Ŭ���� ��ť�� ���� ����
        �����ڷ�, T, function Args�� �ޱ�
*/

namespace sh::Utility {

class JobPoolBase;
class Job : public std::enable_shared_from_this<Job> {
  using Caller = std::function<void()>;

 public:
  Job(std::shared_ptr<JobPoolBase> jobPoolRef = nullptr);
  Job(Caller&& caller, std::shared_ptr<JobPoolBase> jobPoolRef = nullptr);

  void Set(Caller&& caller, std::shared_ptr<JobPoolBase> jobPoolRef = nullptr);

  void Set(Caller&& caller);

  void Execute() { m_job(); }

  void ReturnToJobPool();  // ������ JobPool�� ����, ���ٸ� �׳� ����

 private:
  Caller m_job;

  // ���� Ǯ�� �����ؼ� Job�� �� �����ߴٸ� �ش� Ǯ�� �ٽ� �� �� �ְ�
  std::weak_ptr<JobPoolBase> m_jobPool;  // weak�� ���ϸ� pool <=> job ��ȯ ����

 public:
  template <typename T, typename... Args>
  static Caller GenerateCaller(std::function<void(Args...)> func,
                               std::shared_ptr<T> owner = nullptr,
                               Args... args) {
    auto argLen = sizeof...(args);
    if (nullptr == owner) {
      if (argLen) {
        return [=]() { func(args...); };
      }
      return [=]() { func(); };
    }
    if (argLen) {
      return [=]() { owner->func(args...); };
    }
    return [=]() { owner->func(); };
  }
};
}  // namespace sh::Utility
