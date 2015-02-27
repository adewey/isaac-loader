#ifndef STATICS_H
#define STATICS_H

#include "utilities.h"

struct PointF {
	float x;
	float y;
};

#include "Item.h"
#include "Resource.h"
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
/*0x8AAC*/	char _startSeed[0x10];
/*0x8ABC*/	char _unk8ABC[0x21AE8];
/*0x2A5A4*/	void *_banner;
} PLAYERMANAGER;

GLOBAL DWORD gdwPlayerManager;
GLOBAL DWORD gdwGetPlayerEntity;
GLOBAL Player *GetPlayerEntity();
GLOBAL PlayerManager *GetPlayerManager();
GLOBAL char *GetIsaacVersion();

//for a one liner use second_line, two liner second_line and first_line, three liner third_line, second_line, and first_line in these orders!
GLOBAL void show_fortune_banner(char *first_line, char *second_line, char *third_line);
GLOBAL void show_item_banner(char *first_line, char *second_line, bool style_second_line, bool is_bottom_banner);

void InitStatics();

#endif
