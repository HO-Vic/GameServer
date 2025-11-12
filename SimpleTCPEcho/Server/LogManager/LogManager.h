#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <string>
#include <memory>
#include <cstdint>
/*
        log는 외부 라이브러리 spdlog를 사용
*/
// log

namespace SimpleTCP {
using LogLevel = spdlog::level::level_enum;
class LogManager : public sh::Utility::SingletonBase<LogManager> {
 public:
  void StartLogger(const std::string& logName, const std::string& outputDir, const std::string& fileName, const LogLevel level, const std::string& logMode);

  template <typename... Args>
  void WriteLog(const spdlog::level::level_enum logLevel, const spdlog::format_string_t<Args...> logData, Args... args) {
    m_combinedLog->log(logLevel, logData, std::forward<Args>(args)...);
    switch (logLevel) {
      case spdlog::level::trace:
      case spdlog::level::debug:
      case spdlog::level::info: {
        //
      } break;
      case spdlog::level::warn: {
        m_warningCnt++;
      } break;
      case spdlog::level::err:
      case spdlog::level::critical: {
        m_errCnt++;
      } break;
      default:
        break;
    }
  }

 private:
  std::atomic_int m_warningCnt;
  std::atomic_int m_errCnt;

 public:
  std::shared_ptr<spdlog::logger> m_combinedLog;
};
}  // namespace SimpleTCP

#define LOGGER SimpleTCP::LogManager::GetInstance()
#define START_LOGGER SimpleTCP::LogManager::GetInstance().StartLogger
#define WRITE_LOG SimpleTCP::LogManager::GetInstance().WriteLog