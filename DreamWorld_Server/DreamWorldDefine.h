
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