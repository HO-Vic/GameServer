#pragma once
#include "Connector.h"
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <atomic>
#include <memory>
#include <functional>
#include <tbb/concurrent_priority_queue.h>
#include <Utility/Thread/IWorkerItem.h>
#include <Utility/Job/Job.h>
#include "../../CommonDefine.h"

namespace sh::Utility {
class ThWorkerJob;
}

namespace sh::IO_Engine {
class AsyncConnector
    : public ConnectorBase {
  friend class AsyncConnectEvent;

 private:
  class IntenalTimerJob
      : public Utility::Job {
   public:
    IntenalTimerJob(const Time wakeTime, Utility::Job::Caller&& caller)
        : m_wakeTime(wakeTime), Utility::Job(std::move(caller)) {
    }

    Time GetWakeTime() const {
      return m_wakeTime;
    }

    bool IsReady() const {
      return clock::now() <= m_wakeTime;
    }

   private:
    Time m_wakeTime;
  };

  using TimeOutJobPtr = std::unique_ptr<IntenalTimerJob>;
  class IntenalTimer {
    friend class AsyncConnector;
    struct InternalTimerComp {
      bool operator()(const TimeOutJobPtr& left, const TimeOutJobPtr& right) {
        return left->GetWakeTime() > right->GetWakeTime();
      }
    };

   public:
    IntenalTimer();

    ~IntenalTimer();

    void Start();

    void InsertTimerJob(TimeOutJobPtr&& jobPtr);

   private:
    void TimeOutFunc(std::stop_token stopToken);

   private:
    std::unique_ptr<std::jthread> m_timeOutThread;
    // Connector를 여러 쓰레드에서 접근할 가능성이 있을까??...?
    // std::priority_queue<TimeOutJobPtr, std::vector<TimeOutJobPtr>, InternalTimerComp> m_timerJob;
    tbb::concurrent_priority_queue<TimeOutJobPtr, InternalTimerComp> m_timerJob;
  };

 public:
  AsyncConnector(HANDLE ioHandle, const std::string& ipAddr, uint16_t port, uint16_t inetType, int socketType, int protocolType, const MS timeOutThreshold);

  virtual bool TryConnect(ConnectCompleteHandler successHandle, ConnectFailHandler failHandle) override;

  void InsertTimerJob(std::function<void()>&& timeoutCaller);

 private:
  HANDLE m_ioHandle;
  std::unique_ptr<IntenalTimer> m_intenalTimer;
  MS m_timeOutThreshold;

  LPFN_CONNECTEX ConnectEx;  // 최초 1회 호출
};

class AsyncConnectEvent
    : public Utility::IWorkerItem {
  enum class STATE : char {
    TRY_CONNECT = 1,  // 1 -> 2, 1->3으로 만 STATE 변경 가능 => CAS로
    CONNECTED = 2,
    TIMEOUT = 3
  };

 public:
  AsyncConnectEvent(ConnectCompleteHandler&& successHandle, ConnectFailHandler&& failHandle, LPFN_CONNECTEX connectEx, const SOCKADDR_IN connectAddr);

  bool TryConnect(HANDLE ioHandle, Utility::ThWorkerJob* thWorkerJob, uint16_t inetType, int socketType, int protocolType, AsyncConnector& connector);

  virtual void Execute(Utility::ThWorkerJob* workerJob, const DWORD ioByte, const uint64_t errorCode) override;

 private:
  std::function<void()> GetTimeOutFunc();

  void TimeOut();

 private:
  SOCKET m_socket;
  ConnectCompleteHandler m_successHandle;
  ConnectFailHandler m_failHandle;
  SOCKADDR_IN m_connectAddr;
  LPFN_CONNECTEX ConnectEx;
  uint16_t m_tryCnt;
  sh::Utility::ThWorkerJob* m_workerJob;
  std::atomic<STATE> m_connectingState;
};
}  // namespace sh::IO_Engine
