#pragma once
#include <pch.h>
#include <SingletonBase/Singleton.h>
/*
	log는 외부 라이브러리 spdlog를 사용
*/

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