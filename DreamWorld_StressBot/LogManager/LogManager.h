#pragma once
#include <Utility/SingletonBase/Singleton.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <memory>
#include <cstdint>
/*
        log는 외부 라이브러리 spdlog를 사용
*/
// log
namespace spdlog {
class logger;
}  // namespace spdlog

namespace Stress {
using logLevel = spdlog::level::level_enum;
class LogManager : public sh::Utility::SingletonBase<LogManager> {
 public:
  void StartLogger(const std::string& logName, const std::string& outputDir, const std::string& fileName, const spdlog::level::level_enum level, const std::string& logMode);

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
}  // namespace Stress

#define LOGGER Stress::LogManager::GetInstance()
#define START_LOGGER Stress::LogManager::GetInstance().StartLogger
#define WRITE_LOG Stress::LogManager::GetInstance().WriteLog