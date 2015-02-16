#ifndef STATICS_H
#define STATICS_H

#include "utilities.h"

struct PointF {
	float x;
	float y;
};

#include "Player.h"



enum Curses {
	None = 0,
	Darkness = 1,
	Labyrinth = 2,
	Lost = 4,
	Unknown = 8,
	Cursed = 16,
	Maze = 32,
	Blind = 64,
};

typedef struct PlayerManager
{
/*0x0000*/	int _floorNo;
/*0x0004*/	bool _alternateFloor;
/*0x0008*/	union {
	Curses _curses;
	int i_curses;
};
/*0x000C*/	char _unk000C;
/*0x000D*/	bool _seeForever;
/*0x000E*/	char _unk000E[0x2]; // to fix alignment
/*0x0010*/	char _unk0010[0x8A9C];
/*0x8AAC*/	char _startSeed[10];
} PLAYERMANAGER, *PPLAYERMANAGER;

GLOBAL bool gbAttached;

GLOBAL Player *GetPlayerEntity();
GLOBAL PPLAYERMANAGER GetPlayerManager();

#endif
