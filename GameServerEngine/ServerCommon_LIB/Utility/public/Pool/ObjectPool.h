#pragma once
#include <pch.h>

namespace sh::Utility {
template <typename T>
class ObjectPool {
 public:
  ObjectPool(const uint32_t defaultPoolSize)
      : m_totalCnt(defaultPoolSize),
        m_totalUsingCnt(0),
        m_addedCnt(0),
        m_usingCnt(0) {
  }

  virtual ~ObjectPool() {
    std::lock_guard<std::mutex> lg(m_lock);
    while (!m_pools.empty()) {
      delete m_pools.top();
      m_pools.pop();
    }
  }

  template <typename... Args>
  std::shared_ptr<T> MakeShared(Args&&... args) {
    T* ptr = nullptr;
    {
      std::lock_guard<std::mutex> lg(m_lock);
      if (!m_pools.empty()) {
        ptr = m_pools.top();
        m_pools.pop();
      }
    }

    if (nullptr == ptr) {
      ptr = new T(std::forward<Args>(args)...);
    } else {
      std::construct_at<T>(ptr, std::forward<Args>(args)...);
    }

    return std::shared_ptr<T>(ptr, Release);
    // return std::make_shared<T>(ptr, Release);//make_shared�� deleter ���� �Ұ�
  }

 private:
  void Release(T* ptr) {
    std::lock_guard<std::mutex> lg(m_lock);
    m_pools.push(ptr);
  }

 private:
  // static�� T���� �Ҵ� ��
  std::mutex m_lock;
  std::stack<T*> m_pools;

  std::atomic<uint32_t> m_totalCnt;       // �� ����
  std::atomic<uint32_t> m_totalUsingCnt;  // ���� ���
  std::atomic<uint32_t> m_usingCnt;       // ���� ��뷮
  std::atomic<uint32_t> m_addedCnt;       // �߰�
};

}  // namespace sh::Utility
