#pragma once
#include<wtypes.h>
#include"include/glm/gtc/matrix_transform.hpp"

#define SERVER_PORT 9000

#define DIRECTION_FRONT 0x10
#define DIRECTION_BACK	0x11
#define DIRECTION_LEFT	0x12
#define DIRECTION_RIGHT 0x13


#define s2cMovePacket 0x00
#define s2cDiffClientInfoPacket 0x01

#define c2sDirectionPacket 0x10
#define s2cDisconnectCleintInfoPacket 0x11

#pragma pack(push, 1)

struct DirectionPacket {
	int size;
	char type;
	WORD direction;
};

struct ChessPiecePosPacket {
	int size;
	char type;
	int id;
	glm::vec3 pos;
};

struct ClientDisConnectPacket {
	int size;
	char type;
	int id;
};

#pragma pack(pop)

#pragma once
#include<wtypes.h>
#include"include/glm/glm.hpp"
#include"include/glm/ext.hpp"
#include"include/glm/gtc/matrix_transform.hpp"

/*

서버와 클라이언트의 공유되는 내용들이 담겨져 있다.

*/
#define SERVER_PORT 9000


//constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

constexpr int W_WIDTH = 8;
constexpr int W_HEIGHT = 8;

// Packet ID, 서버 해석 패킷
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

// 클라 해석 패킷
constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_PLAYER = 3;
constexpr char SC_REMOVE_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;

/*

총 6개의 패킷으로 서버와 클라가 통신

*/

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned int move_time;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
};

struct SC_MOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
	unsigned int move_time;
};

#pragma pack (pop)