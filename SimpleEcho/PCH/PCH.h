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

#endif  // !PCH_H

#ifdef _DEBUG
#pragma comment(lib, "Debug/UtilityD.lib")
#pragma comment(lib, "Debug/IO_EngineD.lib")
#endif  // _DEBUG
#pragma comment(lib, "Ws2_32.lib")