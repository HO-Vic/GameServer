#ifndef PCH_H
#define PCH_H

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <fstream>
#include <string>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#endif  // !PCH_H

#ifdef _DEBUG
#pragma comment(lib, "Debug/UtilityD.lib")
#pragma comment(lib, "Debug/IO_EngineD.lib")
#endif  // _DEBUG
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MsWSock.lib")

#ifdef _DEBUG
#pragma comment(lib, "Debug/spdLog/spdlogd.lib")
#else
#pragma comment(lib, "Release/spdLog/spdlog.lib")
#endif  // _DEBUG