#ifdef _DEBUG
#pragma comment(lib, "Debug/UtilityD.lib")
#pragma comment(lib, "Debug/IO_EngineD.lib")
#pragma comment(lib, "Debug/spdlogd.lib")
#else
#pragma comment(lib, "Release/Utility.lib")
#pragma comment(lib, "Release/IO_Engine.lib")
#pragma comment(lib, "Release/spdlog.lib")
#endif  // _DEBUG
#pragma comment(lib, "ws2_32.lib")