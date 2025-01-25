#pragma once
#include <pch.h>
#include <SingletonBase/Singleton.h>

/*
        **현재 시간 기준은 DB 시간.**

        시간에 관해서 계산하는 클래스
        hh:mm:ss
        year -> 2024/24~
        month -> [1~12]
        week -> 어떤 년도로 부터 몇 주차인지 등등을 계산
*/

namespace sh::Utility {
class TimeCalculator final : public SingletonBase<TimeCalculator> {
 private:
  TimeCalculator();
  ~TimeCalculator() {
  }

  TimeCalculator(const TimeCalculator&) = delete;
  TimeCalculator& operator=(const TimeCalculator&) = delete;

 public:
  bool Init();

 private:
};
}  // namespace sh::Utility
