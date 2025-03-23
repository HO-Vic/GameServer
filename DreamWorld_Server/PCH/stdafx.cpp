#include "stdafx.h"
#include "../DreamWorldDefine.h"

#pragma warning (disable:4996)

void PrintCurrentTime()
{
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//std::cout << std::ctime(&currentTime);
}

void DisplayWsaGetLastError(const int& wsaErrcode)
{
	LPWSTR lpMsgBuf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, wsaErrcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpMsgBuf, 0, NULL);
	if (nullptr == lpMsgBuf) return;
	spdlog::critical("WSAErrorCode: {}, Error Message: {}", wsaErrcode, ConvertWideStringToString(lpMsgBuf));
	LocalFree(lpMsgBuf);
}
