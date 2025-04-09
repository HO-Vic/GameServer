#pragma once

namespace sh::Utility {
template <class T>
class SingletonBase {
 protected:
  SingletonBase() = default;

  // 싱글톤이기 때문에, 복사가 일어나면 안 됨.
  SingletonBase(const SingletonBase&) = delete;
  SingletonBase& operator=(const SingletonBase&) = delete;

 public:
  static T& GetInstance() {
    // thread-safe??
    static T instance;
    return instance;
  }
};
}  // namespace sh::Utility