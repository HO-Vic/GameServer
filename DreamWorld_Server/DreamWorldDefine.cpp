#include "stdafx.h"
#include "DreamWorldDefine.h"
#include <atlconv.h>
#include <Utility/BuildMsg/BuildMsg.h>

std::string ConvertWideStringToString(const wchar_t* wstr) {
  BUILD_MESSAGE(__FILE__, __LINE__, "중복 정의 테스트1");
  USES_CONVERSION;
  return std::string(W2A(wstr));
}

std::wstring ConvertStringToWideString(const char* str) {
  BUILD_MESSAGE(__FILE__, __LINE__, "중복 정의 테스트2");
  USES_CONVERSION;
  return std::wstring(A2W(str));
}
