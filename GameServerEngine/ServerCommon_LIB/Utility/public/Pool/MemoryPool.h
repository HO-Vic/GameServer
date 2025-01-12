#pragma once
#include <SingletonBase/Singleton.h>

namespace sh::Utility {
class MemoryPool  // 메모리 헤더 집합
{
  struct MemoryHeader {
   public:
    MemoryHeader(const uint32_t size)
        : m_allocSize(size) {
    }

    const uint32_t GetSize() const {
      return m_allocSize;
    }

    static MemoryHeader* AttachHeader(void* allocPtr, const uint32_t size);

    static MemoryHeader* DetachHeader(void* dataPtr);

   private:
    uint32_t m_allocSize;
  };

 public:
  MemoryPool(const uint32_t allocSize)
      : m_allocSize(allocSize) {
  }

  void Push(void* ptr);

  void* Get(const uint32_t allocSize);

 private:
  std::mutex m_lock;
  std::stack<MemoryHeader*> m_blocks;
  uint32_t m_allocSize;
};

class MemoryPoolTable : public SingletonBase<MemoryPoolTable> {
  using MemoryPools = std::vector<std::shared_ptr<MemoryPool>>;
  constexpr static uint16_t MAX_MEM_POOL_SIZE = 4096;

 private:
  MemoryPoolTable() = default;
  MemoryPoolTable(const MemoryPoolTable&) = delete;
  MemoryPoolTable(MemoryPoolTable&&) = delete;

 public:
  bool Init();

  template <typename T, typename... Args>
  T* Get(Args... args) {
    if (sizeof(T) > MAX_MEM_POOL_SIZE) {
      return new T(std::forward(args...));
    }
    MemoryPool& pool = *m_memoryPoolTable[sizeof(T) - 1];
    auto allocedMem = pool.Get(sizeof(T));
    return std::construct_at(reinterpret_cast<T*>(allocedMem),
                             std::forward(args...));
  }

  template <typename T>
  void Release(T* ptr) {
    if (sizeof(T) > MAX_MEM_POOL_SIZE) {
      delete ptr;
      return;
    }

    MemoryPool& pool = *m_memoryPoolTable[sizeof(T) - 1];
    pool.Push(ptr);
  }

 private:
  MemoryPools m_memoryPoolTable;  // idx:allocSize, val: memoryPool
};

}  // namespace sh::Utility
