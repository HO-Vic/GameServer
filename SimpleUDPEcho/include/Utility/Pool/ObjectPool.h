#pragma once
#include <atomic>
#include <mutex>
#include <stack>
#include <memory>
#include <set>
#include <functional>

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
    bool isPool = false;
    {
      std::lock_guard<std::mutex> lg(m_lock);
      if (!m_pools.empty()) {
        ptr = m_pools.top();
        m_pools.pop();
        isPool = true;
      }
    }

    if (nullptr == ptr) {
      m_addedCnt++;
      m_totalCnt++;
      ptr = new T(std::forward<Args>(args)...);
    } else {
      std::construct_at<T>(ptr, std::forward<Args>(args)...);
    }
#ifdef _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_usingLock);
      if (m_usingSet.count(ptr) > 0) {
        int i = 0;
        i++;
      }
      m_usingSet.insert(ptr);
      m_usingCnt++;
    }
#else
    m_usingCnt++;
#endif  // _DEBUG
    // return std::shared_ptr<T>(ptr, std::move([this](T* ptr) {
    //                             this->Release(ptr);
    //                           }));  // return std::make_shared<T>(ptr, Release);//make_shared는 deleter 지정 불가
    return std::shared_ptr<T>(ptr, std::bind(&ObjectPool<T>::Release, this, std::placeholders::_1));
  }

  template <typename... Args>
  std::unique_ptr<T, std::function<void(T*)>> MakeUnique(Args&&... args) {
    T* ptr = nullptr;
    bool isPool = false;
    {
      std::lock_guard<std::mutex> lg(m_lock);
      if (!m_pools.empty()) {
        ptr = m_pools.top();
        m_pools.pop();
        isPool = false;
      }
    }

    if (nullptr == ptr) {
      m_addedCnt++;
      m_totalCnt++;
      ptr = new T(std::forward<Args>(args)...);
    } else {
      std::construct_at<T>(ptr, std::forward<Args>(args)...);
    }
#ifdef _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_usingLock);
      if (m_usingSet.count(ptr) > 0) {
        int i = 0;
        i++;
      }
      m_usingSet.insert(ptr);
      m_usingCnt++;
    }
#else
    m_usingCnt++;
#endif  // _DEBUG
    // return std::unique_ptr<T, std::function<void(T*)>>(ptr, std::move([this](T* ptr) {
    //                                                      this->Release(ptr);
    //                                                    }));  // return std::make_shared<T>(ptr, Release);//make_shared는 deleter 지정 불가
    return std::unique_ptr<T, std::function<void(T*)>>(ptr, std::bind(&ObjectPool<T>::Release, this, std::placeholders::_1));
  }

  uint64_t GetTotalCnt() const {
    return m_totalCnt;
  }

  uint64_t GetUsingCnt() const {
    return m_usingCnt;
  }

  uint64_t GetAddedCnt() const {
    return m_addedCnt;
  }

 private:
  void Release(T* ptr) {
    std::destroy_at<T>(ptr);
#ifdef _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_usingLock);
      if (m_usingSet.count(ptr) == 0) {
        int i = 0;
        i++;
      }
      m_usingSet.erase(ptr);
      m_usingCnt--;
    }
#else
    m_usingCnt--;
#endif  // _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_lock);
      m_pools.push(ptr);
    }
  }

 private:
  // static은 T마다 할당 됨
  std::mutex m_lock;
  std::stack<T*> m_pools;
  std::atomic<uint64_t> m_totalCnt;       // 총 갯수
  std::atomic<uint64_t> m_totalUsingCnt;  // 누적 사용
  std::atomic<uint64_t> m_usingCnt;       // 현재 사용량
  std::atomic<uint64_t> m_addedCnt;       // 추가

#ifdef _DEBUG
  std::mutex m_usingLock;
  std::set<T*> m_usingSet;
#endif  // _DEBUG
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
      m_addedCnt++;
      m_totalCnt++;
      ptr = new T(std::forward<Args>(args)...);
    } else {
      std::construct_at<T>(ptr, std::forward<Args>(args)...);
    }
#ifdef _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_usingLock);
      m_usingSet.insert(ptr);
    }
#endif  // _DEBUG

    m_usingCnt++;
    return ptr;
  }

  void Release(T* ptr) {
    std::destroy_at(ptr);
    {
      std::lock_guard<std::mutex> lg(m_lock);
      m_pools.push(ptr);
    }
#ifdef _DEBUG
    {
      std::lock_guard<std::mutex> lg(m_usingLock);
      m_usingSet.erase(ptr);
    }
#endif  // _DEBUG

    m_usingCnt--;
  }

  uint64_t GetTotalCnt() const {
    return m_totalCnt;
  }

  uint64_t GetUsingCnt() const {
    return m_usingCnt;
  }

  uint64_t GetAddedCnt() const {
    return m_addedCnt;
  }

 private:
  // static은 T마다 할당 됨
  std::mutex m_lock;
  std::stack<T*> m_pools;

#ifdef _DEBUG
  std::mutex m_usingLock;
  std::set<T*> m_usingSet;
#endif  // _DEBUG

  std::atomic<uint64_t> m_totalCnt;       // 총 갯수
  std::atomic<uint64_t> m_totalUsingCnt;  // 누적 사용
  std::atomic<uint64_t> m_usingCnt;       // 현재 사용량
  std::atomic<uint64_t> m_addedCnt;       // 추가
};
}  // namespace sh::Utility
