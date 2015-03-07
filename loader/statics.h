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
/*0x000000*/	int _floorNo;
/*0x000004*/	BOOL _alternateFloor;
/*0x000008*/	union {
					Curses _curses;
					int i_curses;
				};
/*0x00000C*/	char _unk000C;
/*0x00000D*/	bool _seeForever;
/*0x00000E*/	char _unk000E[0x2]; // to fix alignment
/*0x000010*/	char _unk0010[0x8A9C];
/*0x008AAC*/	char _startSeed[0x10];
/*0x008ABC*/	char _unk8ABC[0x21AE8];
/*0x02A5A4*/	int _HUD;
/*0x02A5A8*/	char _unk2A5A8[0xE3260];
/*0x10D808*/	int _challenge_id;
/*0x10D80C*/	char _unk10D80C[0xB0];
/*0x10D8BC*/	BOOL _disable_achievements;
/*0x10D8C0*/	char unk10D8C0[0x4];
/*0x10D8C4*/	BOOL _hard_mode;
} PLAYERMANAGER;

GLOBAL DWORD gdwPlayerManager;
GLOBAL DWORD gdwGetPlayerEntity;
GLOBAL void ToggleCurseOfTheBlind();
GLOBAL Player *GetPlayerEntity();
GLOBAL PlayerManager *GetPlayerManager();
GLOBAL char *GetIsaacVersion();

//for a one liner use second_line, two liner second_line and first_line, three liner third_line, second_line, and first_line in these orders!
GLOBAL void show_fortune_banner(char *first_line, char *second_line, char *third_line);
GLOBAL void show_item_banner(char *first_line, char *second_line, bool style_second_line, bool is_bottom_banner);

void InitStatics();

#endif
