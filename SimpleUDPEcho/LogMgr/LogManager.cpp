#include "pch.h"
#include "LogManager.h"
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <ctime>

namespace sh::UDP_Echo {
void LogManager::StartLogger(const std::string& logName, const std::string& outputDir, const std::string& fileName, const spdlog::level::level_enum level, const std::string& logMode) {
  m_warningCnt = 0;
  m_errCnt = 0;

  auto now = std::chrono::system_clock::now();
  auto time_point = std::chrono::system_clock::to_time_t(now);
  struct std::tm parts{};
  localtime_s(&parts, &time_point);

  std::stringstream currentTimeStrSteam;
  currentTimeStrSteam << std::put_time(&parts, "logs/%m-%d-%H.%M.%S.txt");
  std::string timeStr = currentTimeStrSteam.str();

  std::string outputFileName(outputDir + fileName + currentTimeStrSteam.str() + ".txt");

  if (logMode.compare("Console") == 0) {
    auto consoleLogger = spdlog::stdout_color_mt(logName + "_console");
    m_combinedLog = std::make_shared<spdlog::logger>(logName + "_combineLog", spdlog::sinks_init_list({consoleLogger->sinks().front()}));
  } else if (logMode.compare("File") == 0) {
    auto txtLogger = spdlog::basic_logger_mt(logName, outputFileName);
    m_combinedLog = std::make_shared<spdlog::logger>(logName + "_combineLog", spdlog::sinks_init_list({txtLogger->sinks().front()}));
  } else if (logMode.compare("ConsoleFile") == 0) {
    auto txtLogger = spdlog::basic_logger_mt(logName, outputFileName);
    auto consoleLogger = spdlog::stdout_color_mt(logName + "_console");
    m_combinedLog = std::make_shared<spdlog::logger>(logName + "_combineLog", spdlog::sinks_init_list({txtLogger->sinks().front(), consoleLogger->sinks().front()}));
  }

  m_combinedLog->set_level(level);
  WRITE_LOG(level::info, "{}({}) > Logger Start", __FUNCTION__, __LINE__);
}
}  // namespace sh::UDP_Echo