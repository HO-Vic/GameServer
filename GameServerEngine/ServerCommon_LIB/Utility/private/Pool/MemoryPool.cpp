#include "pch.h"
#include "Pool/MemoryPool.h"

namespace sh::Utility {
MemoryPool::MemoryHeader* MemoryPool::MemoryHeader ::AttachHeader(void* allocPtr, const uint32_t size) {  // �Ҵ�� ptr�� ���ؼ� �޸� ��� ���̱�
  auto memHeader = reinterpret_cast<MemoryHeader*>(allocPtr);
  std::construct_at(memHeader, size);  // c++20�� �߰��� ���, placement new���� �����ϴ�
  return ++memHeader;                  // �޸� ��� ����������, data�������� ����
}

MemoryPool::MemoryHeader* MemoryPool::MemoryHeader::DetachHeader(void* dataPtr) {
  return reinterpret_cast<MemoryHeader*>(dataPtr) - 1;  // ������ ptr -> �޸� ��� ptr�� ��ȯ �ǵ�
}

void MemoryPool::Push(void* ptr) {
  std::lock_guard<std::mutex> lg(m_lock);
  auto returnPtr = MemoryHeader::DetachHeader(ptr);
  m_blocks.push(returnPtr);
}

void* MemoryPool::Get(const uint32_t allocSize) {
  MemoryHeader* ptr = nullptr;
  {
    std::lock_guard<std::mutex> lg(m_lock);
    ptr = m_blocks.top();
    m_blocks.pop();
  }
  if (nullptr == ptr) {
    ptr = reinterpret_cast<MemoryHeader*>(malloc(allocSize + sizeof(MemoryHeader)));
  }
  return MemoryHeader::AttachHeader(ptr, allocSize);
}

bool MemoryPoolTable::Init() {
  // �޸� ����
  // �Ҵ� �޸� - ������
  // 8~256 => 8
  // 256+32 ~ 1024 => 32
  // 1024+128~2048 => 128
  // 2048+256 ~ 4096 => 256
  m_memoryPoolTable.resize(4095);

  uint16_t allocatedSize = 8;
  uint16_t idx = 0;
  for (allocatedSize = 8; allocatedSize += 8; allocatedSize <= 256) {
    auto memPool = std::make_shared<MemoryPool>(allocatedSize);
    for (; idx + 1 <= allocatedSize; idx++) {
      m_memoryPoolTable[idx] = memPool;
    }
  }

  for (allocatedSize = 256 + 32; allocatedSize += 32; allocatedSize <= 1024) {
    auto memPool = std::make_shared<MemoryPool>(allocatedSize);
    for (; idx + 1 <= allocatedSize; idx++) {
      m_memoryPoolTable[idx] = memPool;
    }
  }

  for (allocatedSize = 1024 + 128; allocatedSize += 128;
       allocatedSize <= 2048) {
    auto memPool = std::make_shared<MemoryPool>(allocatedSize);
    for (; idx + 1 <= allocatedSize; idx++) {
      m_memoryPoolTable[idx] = memPool;
    }
  }

  for (allocatedSize = 2048 + 256; allocatedSize += 256;
       allocatedSize <= 4096) {
    auto memPool = std::make_shared<MemoryPool>(allocatedSize);
    for (; idx + 1 <= allocatedSize; idx++) {
      m_memoryPoolTable[idx] = memPool;
    }
  }
  return true;
}

}  // namespace sh::Utility
