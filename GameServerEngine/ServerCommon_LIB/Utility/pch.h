// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H
// 여기에 미리 컴파일하려는 헤더 추가
#include <array>
#include <list>
#include <vector>
#include <stack>
#include <queue>

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>

#include <memory>

#include<thread>
#include <stop_token>

//lf 컨테이너
#include <tbb/tbb.h>

#include <atomic>

#endif //PCH_H


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

#define STRINGIFY(x) #x
#define CON_CAT(x, y) x ## y
#define BUILD_MESSAGE(file, line, msg) __pragma(message(\
	file## "(" ## STRINGIFY(line) ## "): "## msg \
	))
