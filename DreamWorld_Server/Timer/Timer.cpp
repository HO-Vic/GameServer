#include "stdafx.h"
#include "Timer.h"
#include <queue>
#include <vector>
#include <Utility/Job/Job.h>
#include "TimerJob.h"
#include "../ThreadManager/ThreadManager.h"

namespace DreamWorld {
void Timer::Start() {
  ThreadManager::GetInstance().InsertThread([](std::stop_token stopToken) {
    Timer::GetInstance().TimerThreadFunc(stopToken);
  });
}

void Timer::InsertTimerEvent(std::unique_ptr<TimerJob, std::function<void(TimerJob*)>>&& timer) {
  m_timerQueue.push(std::move(timer));
}

void Timer::TimerThreadFunc(std::stop_token stopToken) {
  /*
          얼마 남지 않은 타이머 이벤트에 대해서 임시 타이머 큐에 저장
          concurrency_priority_queue에 다시 삽입하는거보다는 임시로 저장하고
          곧 수행되기 때문에, 다음 수행 때 바로 실행에 가까움
          top으로 우선순위 높은 객체 볼 수 있음.
  */
  std::priority_queue<TimerJobPtr, std::vector<TimerJobPtr>, Timer::TimerQueueComp> immediateTimer;
  constexpr DreamWorld::MS PUSH_IMMEDIATE_TIMER_QUEUE_TIME = DreamWorld::MS(4);  // 어느정도 시간이 적당할지는 생각해야할듯...
  while (true) {
    if (stopToken.stop_requested()) {
      break;
    }
    while (!immediateTimer.empty()) {
      // 임시큐에서 빼지 않고 최우선 타이머 이벤트를 볼 수 있음
      auto immediateTimerEvent = immediateTimer.top().get();  // unique_ptr이기 때문에, get으로만하고 pop
      if (!immediateTimerEvent->IsReady()) {                  // 즉시 수행 불가능이라면 임시 타이머 큐 객체는 종료
        break;
      }
      // 임시 타이머 객체니 수행 가능
      immediateTimerEvent->Execute();
      immediateTimer.pop();
    }

    if (m_timerQueue.empty()) {
      // 타이머 이벤트 수행할게 없어 다른 쓰레드에 양보
      std::this_thread::yield();
      continue;
    }
    while (true) {
      std::unique_ptr<TimerJob, std::function<void(TimerJob*)>> currentEvent = nullptr;
      bool isSuccess = m_timerQueue.try_pop(currentEvent);
      if (!isSuccess) {  // 이벤트를 못 가져왔다면 다른 쓰레드에 양보
        // Sleep(1);
        std::this_thread::yield();
        break;
      }

      if (currentEvent->IsReady()) {  // 수행할 시간이 됐다면 수행
        currentEvent->Execute();
        continue;
      }
      // 아니라면 다시 삽입
      DreamWorld::MS restTime = currentEvent->GetRestTimeForReady();
      if (restTime <= PUSH_IMMEDIATE_TIMER_QUEUE_TIME) {  // 기준 시간보다 적은 시간이 남았다면, 임시 큐에 삽입
        immediateTimer.push(std::move(currentEvent));
      } else {
        m_timerQueue.push(std::move(currentEvent));  // 아니라면 concurrent에 삽입
      }
      Sleep(1);
      break;
    }
  }
}
}  // namespace DreamWorld