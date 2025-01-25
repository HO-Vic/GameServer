#pragma once
#include <PCH/pch.h>

/*
        ������ Ǯ�� �����ϴ� �����̳�
        �����带 �߰� ���� => �̰� ���
        �����ڿ����� thread vector ������ ���, Add���� �߰��ϴ°� ������
        ���ÿ� ���� �����忡�� ThreadPool�� ������ �߰��� �������� ����.
*/

// template<typename Args>
// std::function<void(std::stop_token&, Args...)> threadFunctor;

namespace sh::Utility {

class ThreadPool {
 public:
  ThreadPool(const int threadNum = 1);

  virtual ~ThreadPool();

  // jthread�� stop_token�� ���ڷ� ���� �� �ְ�
  template <typename... Args>
  void InsertThread(std::function<void(std::stop_token&, Args...)>&& jthreadFunctor, Args&&... args) {
    m_threads.push_back(std::jthread(std::forward(jthreadFunctor), std::forward<Args>(args)...));
  }

  void ForceStop();

 private:
  std::vector<std::jthread> m_threads;
};
}  // namespace sh::Utility
