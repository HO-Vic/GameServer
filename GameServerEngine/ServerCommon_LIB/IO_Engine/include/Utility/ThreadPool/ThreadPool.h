#pragma once
#include <PCH/pch.h>

/*
        쓰레드 풀을 관리하는 컨테이너
        쓰레드를 추가 삭제 => 이건 고민
        생성자에서는 thread vector 공간만 잡고, Add에서 추가하는게 나려나
        동시에 여러 쓰레드에서 ThreadPool에 쓰레드 추가는 생각하지 않음.
*/

// template<typename Args>
// std::function<void(std::stop_token&, Args...)> threadFunctor;

namespace sh::Utility {

class ThreadPool {
 public:
  ThreadPool(const int threadNum = 1);

  virtual ~ThreadPool();

  // jthread의 stop_token을 인자로 가질 수 있게
  template <typename... Args>
  void InsertThread(std::function<void(std::stop_token&, Args...)>&& jthreadFunctor, Args&&... args) {
    m_threads.push_back(std::jthread(std::forward(jthreadFunctor), std::forward<Args>(args)...));
  }

  void ForceStop();

 private:
  std::vector<std::jthread> m_threads;
};
}  // namespace sh::Utility
