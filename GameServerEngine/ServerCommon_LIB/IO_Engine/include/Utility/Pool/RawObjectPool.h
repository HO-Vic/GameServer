#pragma once
#include <pch.h>

namespace sh::Utility {
template <typename T>
class RawObjectPool {
 public:
  RawObjectPool()
      : m_totalCnt(0),
        m_totalUsingCnt(0),
        m_addedCnt(0),
        m_usingCnt(0) {
  }

  RawObjectPool(const uint32_t defaultPoolSize)
      : m_totalCnt(defaultPoolSize),
        m_totalUsingCnt(0),
        m_addedCnt(0),
        m_usingCnt(0) {
  }

  virtual ~RawObjectPool() {
    std::lock_guard<std::mutex> lg(m_lock);
    while (!m_pools.empty()) {
      delete m_pools.top();
      m_pools.pop();
    }
  }

  void Init(const uint32_t poolSize) {
    m_totalCnt = poolSize;
  }

  template <typename... Args>
  T* GetObjectPtr(Args&&... args) {
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

    return ptr;
  }

  void Release(T* ptr) {
    std::lock_guard<std::mutex> lg(m_lock);
    m_pools.push(ptr);
  }

 private:
  // static은 T마다 할당 됨
  std::mutex m_lock;
  std::stack<T*> m_pools;

  std::atomic<uint32_t> m_totalCnt;       // 총 갯수
  std::atomic<uint32_t> m_totalUsingCnt;  // 누적 사용
  std::atomic<uint32_t> m_usingCnt;       // 현재 사용량
  std::atomic<uint32_t> m_addedCnt;       // 추가
};

}  // namespace sh::Utility
