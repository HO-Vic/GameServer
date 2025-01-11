#pragma once
#include <pch.h>

/*
        함수자를 가질 클래스 잡큐를 위한 래퍼
        생성자로, T, function Args를 받기
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

  void ReturnToJobPool();  // 본인을 JobPool로 삽입, 없다면 그냥 리턴

 private:
  Caller m_job;

  // 잡의 풀을 저장해서 Job을 다 실행했다면 해당 풀로 다시 갈 수 있게
  std::weak_ptr<JobPoolBase> m_jobPool;  // weak로 안하면 pool <=> job 순환 참조

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
