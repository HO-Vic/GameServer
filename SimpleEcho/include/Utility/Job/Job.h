#pragma once
#include <functional>
/*
        함수자를 가질 클래스 잡큐를 위한 래퍼
        생성자로, T, function Args를 받기
*/

namespace sh::Utility {
class Job : public std::enable_shared_from_this<Job> {
 public:
  using Caller = std::function<void()>;
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
  // 소유권이 궁금해지네
  // ㄴ 람다 캡쳐
  // "[=]" : 복사
  // "[&]" : 참조
  // 아래는 shared_ptr이 복사이기 때문에, 소유권 유지
  //  template <typename Func, typename T, typename... Args>
  //  static Caller GenerateCaller(Func func, std::shared_ptr<T>& owner, const bool isWeak, Args... args) {
  //    if (nullptr == owner) {
  //      return [=]() { func(std::forward<Args>(args)...); };
  //    } else if (isWeak) {
  //      std::weak_ptr<T> weakPtr = owner;
  //      return [=]() {
  //        auto strongPtr = weakPtr.lock();
  //        if (nullptr == strongPtr) {
  //          return;
  //        }
  //        owner->func(std::forward<Args>(args)...);
  //      };
  //    }
  //    return [=]() { owner->func(std::forward<Args>(args)...); };
  //  }
};
}  // namespace sh::Utility
