#pragma once
#include <string>
#include <chrono>

#ifndef DreamWorldDefine
#define DreamWorldDefine

#define _chrono std::chrono
#define chrono_clock std::chrono::steady_clock

enum class DIRECTION : char {
  IDLE = 0x00,
  FRONT = 0x01,
  RIGHT = 0x02,
  LEFT = 0x04,
  BACK = 0x08
};

enum class ROTATE_AXIS : char {
  X,
  Y,
  Z
};

enum class ROLE : char {
  NONE_SELECT = 0x00,
  WARRIOR = 0x01,
  MAGE = 0x02,
  TANKER = 0x04,
  ARCHER = 0x08,
};

enum class ROOM_STATE : char {
  ROOM_COMMON,
  ROOM_BOSS,
  ROOM_END
};

enum class PLAYER_STATE : char {
  LOBBY,
  MATCH,
  IN_GAME,
  RECONN_FAIL
};

namespace DreamWorld {
using MS = _chrono::milliseconds;
using SEC = _chrono::seconds;

constexpr MS ROOM_UPDATE_TICK = MS(50);
}  // namespace DreamWorld

#endif  // !DreamWorldDefine
        // wchar->char
std::string ConvertWideStringToString(const wchar_t* wstr);
// char->wchar
std::wstring ConvertStringToWideString(const char* str);
