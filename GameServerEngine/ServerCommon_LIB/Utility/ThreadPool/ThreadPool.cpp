#include "pch.h"
#include "ThreadPool.h"

namespace sh {
	namespace Utility {
		ThreadPool::ThreadPool(const int threadNum)
		{
			m_threads.reserve(threadNum);
		}

		ThreadPool::~ThreadPool()
		{
			for (auto& jth : m_threads) {
				jth.join();
			}
		}

		void ThreadPool::ForceStop()
		{
			for (auto& jth : m_threads) {
				jth.request_stop();
			}
		}
	}
}
