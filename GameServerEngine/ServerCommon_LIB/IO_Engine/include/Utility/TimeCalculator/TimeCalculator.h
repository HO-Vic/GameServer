#pragma once
#include <pch.h>
#include <SingletonBase/Singleton.h>

/*
        **���� �ð� ������ DB �ð�.**

        �ð��� ���ؼ� ����ϴ� Ŭ����
        hh:mm:ss
        year -> 2024/24~
        month -> [1~12]
        week -> � �⵵�� ���� �� �������� ����� ���
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
