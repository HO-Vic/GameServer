#pragma once
#include <pch.h>
#include <SingletonBase/Singleton.h>
/*
	log는 외부 라이브러리 spdlog를 사용
*/
//log
#ifdef _DEBUG
#pragma comment (lib, "Debug/spdLog/spdlogd.lib")
#else
#pragma comment (lib, "Release/spdLog/spdlog.lib")
#endif // _DEBUG

#pragma warning (disable:4996)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sh {
	namespace Utility {
		class LogManager : public SingletonBase<LogManager>
		{
		public:
			void StartLogger(const std::string& logName, const std::string& fileName);

			template<typename ...Args>
			void WriteLog(const spdlog::level::level_enum logLevel, const std::string& func, const int line, const std::string& logData, Args... args);
		private:
			static std::atomic_int m_warningCnt;
			static std::atomic_int m_errCnt;
			static std::shared_ptr<spdlog::logger> m_combinedLog;
		};
	}
}

#define LOGGER  LogManager::GetInstance()
#define START_LOGGER  LogManager::GetInstance().StartLogger
#define WRITE_LOG LogManager::GetInstance().WriteLog