#include <pch.h>
#include <Log/LogManager.h>

namespace sh {
	namespace Utility {

		void LogManager::StartLogger(const std::string& logName, const std::string& fileName)
		{
			m_warningCnt = 0;
			m_errCnt = 0;

			auto now = std::chrono::system_clock::now();
			auto time_point = std::chrono::system_clock::to_time_t(now);
			struct std::tm* parts = std::localtime(&time_point);

			std::stringstream currentTimeStrSteam;
			currentTimeStrSteam << std::put_time(parts, "logs/%m-%d-%H.%M.%S.txt");
			std::string timeStr = currentTimeStrSteam.str();

			std::string outputFileName(fileName + currentTimeStrSteam.str() + ".txt");

			auto txtLogger = spdlog::basic_logger_mt(logName, outputFileName);
			auto consoleLogger = spdlog::stdout_color_mt(logName + "_console");
			m_combinedLog = std::make_shared<spdlog::logger>(logName + "_combineLog", spdlog::sinks_init_list({ txtLogger->sinks().front(), consoleLogger->sinks().front() }));
			m_combinedLog->info("loggerStarted");
		}

		template<typename ...Args>
		inline void sh::Utility::LogManager::WriteLog(const spdlog::level::level_enum logLevel, const std::string& func, const int line, const std::string& logData, Args ...args)
		{
			m_combinedLog->log(logLevel, logData, func, line, args...);
			switch (logLevel)
			{
			case spdlog::level::trace:
			case spdlog::level::debug:
			case spdlog::level::info:
			{
				//
			}
			break;
			case spdlog::level::warn:
			{
				m_warningCnt++;
			}
			break;
			case spdlog::level::err:
			case spdlog::level::critical:
			{
				m_errCnt++;
			}
			break;
			default:
				break;
			}
		}
	}
}

//#ifdef _DEBUG
//WRITE_LOG(spdlog::level::info, __FUNCTION__, __LINE__, "%s(),- line.%d, JobPoolName: %s, newPoolCnt: %d", m_name, ++m_newJobCnt);
//#endif // _DEBUG