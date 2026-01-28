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

ThWorkerJob::ThWorkerJob(const WorkerPtr& workItem, WORKER_TYPE type)
    : m_workItem(workItem), m_type(type) {
  ZeroMemory(this, sizeof(OVERLAPPED));
}

ThWorkerJob::ThWorkerJob(WorkerPtr&& workItem, WORKER_TYPE type)
    : m_workItem(workItem), m_type(type) {
  ZeroMemory(this, sizeof(OVERLAPPED));
}

void ThWorkerJob::operator()(const DWORD ioByte, const DWORD errorCode) {
  m_workItem->Execute(this, ioByte, errorCode);
  // ZeroMemory(this, sizeof(OVERLAPPED));
}
}  // namespace sh::Utility