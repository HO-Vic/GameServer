#include "pch.h"
#include "JobQ_MT.h"
#include "../../Job.h"

namespace sh {
	namespace Utility {

		void JobQ_MT::DoJobs(const uint64_t execCnt)
		{
			static constexpr uint64_t MAX_EXEC_CNT = 100;//��� �ٸ� �����忡�� ������ �� ���� �߰��ϸ�, ������ �� ����, �߰� end ����
			uint32_t executeCnt = 0;
			if (ALL_JOB_EXEC == execCnt && executeCnt != MAX_EXEC_CNT) {
				while (!m_jobs.empty()) {
					m_lock.lock();
					JobPtr job = m_jobs.front();
					m_jobs.pop();
					m_lock.unlock();
					job->Execute();
					executeCnt++;
				}
			}
			else {
				while (!m_jobs.empty() && execCnt != executeCnt) {
					m_lock.lock();
					JobPtr job = m_jobs.front();
					m_jobs.pop();
					m_lock.unlock();
					job->Execute();
					executeCnt++;
				}
			}
		}

		void JobQ_MT::InsertJob(std::shared_ptr<Job>& job)
		{
			m_lock.lock();
			m_jobs.push(job);
			m_lock.unlock();
		}
	}
}

