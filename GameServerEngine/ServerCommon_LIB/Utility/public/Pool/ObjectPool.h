#pragma once
#include <atomic>
#include <mutex>
#include <stack>
#include <memory>

namespace sh::Utility {
template <typename T>
class ObjectPool {
 public:
  ObjectPool()
      : m_totalCnt(0),
        m_totalUsingCnt(0),
        m_addedCnt(0),
        m_usingCnt(0) {
  }

  ObjectPool(const uint32_t defaultPoolSize)
      : m_totalCnt(defaultPoolSize),
        m_totalUsingCnt(0),
        m_addedCnt(0),
        m_usingCnt(0) {
    for (uint32_t i = 0; i < m_totalCnt; ++i) {
      m_pools.push(new T);
    }
  }

  virtual ~ObjectPool() {
    std::lock_guard<std::mutex> lg(m_lock);
    while (!m_pools.empty()) {
      delete m_pools.top();
      m_pools.pop();
    }
  }

  void InitSize(const int32_t poolSize) {
    m_totalCnt = poolSize;
    for (uint32_t i = 0; i < m_totalCnt; ++i) {
      m_pools.push(new T);
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

    return std::shared_ptr<T>(ptr, std::move([=](T* ptr) {
                                Release(ptr);
                              }));  // return std::make_shared<T>(ptr, Release);//make_shared는 deleter 지정 불가
  }

  template <typename... Args>
  std::unique_ptr<T, std::function<void(T*)>> MakeUnique(Args&&... args) {
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

    return std::unique_ptr<T, std::function<void(T*)>>(ptr, std::move([=](T* ptr) {
                                                         Release(ptr);
                                                       }));  // return std::make_shared<T>(ptr, Release);//make_shared는 deleter 지정 불가
  }

 private:
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
    for (uint32_t i = 0; i < m_totalCnt; ++i) {
      m_pools.push(new T);
    }
  }

  virtual ~RawObjectPool() {
    std::lock_guard<std::mutex> lg(m_lock);
    while (!m_pools.empty()) {
      delete m_pools.top();
      m_pools.pop();
    }
  }

  void InitSize(const uint32_t poolSize) {
    m_totalCnt = poolSize;
    for (uint32_t i = 0; i < m_totalCnt; ++i) {
      m_pools.push(new T);
    }
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
