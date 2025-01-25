#pragma once
#include <atomic>

namespace sh::Utility {

class SpinLock {
 public:
  SpinLock() : m_condition(false) {}

  void lock() {
    bool EXPECTED = false;
    static constexpr bool DESIRED = true;

    while (!m_condition.compare_exchange_strong(EXPECTED, DESIRED,
                                                std::memory_order_seq_cst))
      ;
  }

  void unlock() { m_condition = false; }

 private:
  std::atomic_bool m_condition;
};
}  // namespace sh::Utility
