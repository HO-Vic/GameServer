#include "pch.h"
#include "Thread/ThWorkerJob.h"
#include "Thread/IWorkerItem.h"

namespace sh::Utility {
ThWorkerJob::ThWorkerJob()
    : m_workItem(nullptr), m_type(WORKER_TYPE::NONE) {
  ZeroMemory(this, sizeof(OVERLAPPED));
}

ThWorkerJob::~ThWorkerJob() {
  m_workItem = nullptr;
}

ThWorkerJob::ThWorkerJob(WokerPtr& workItem, WORKER_TYPE type)
    : m_workItem(workItem), m_type(type) {
  ZeroMemory(this, sizeof(OVERLAPPED));
}

ThWorkerJob::ThWorkerJob(WokerPtr&& workItem, WORKER_TYPE type)
    : m_workItem(workItem), m_type(type) {
  ZeroMemory(this, sizeof(OVERLAPPED));
}

void ThWorkerJob::operator()(const DWORD ioByte) {
  m_workItem->Execute(this, ioByte);
  // ZeroMemory(this, sizeof(OVERLAPPED));
}
}  // namespace sh::Utility