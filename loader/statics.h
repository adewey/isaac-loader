#ifndef STATICS_H
#define STATICS_H

#include "utilities.h"

typedef struct _Entity
{
	/*0x0000*/	char _unk0000[0xC];
	/*0x000C*/	int _id;
	/*0x0010*/	unsigned int _variant;
	/*0x0014*/	int _subtype;
	/*0x0018*/	char _unk0018[0x5C];
	/*0x0074*/	int _tearType;
	/*0x0078*/	char _unk0078[0x23C];
	/*0x02B4*/	float _scaleX;
	/*0x02B8*/	float _scaleY;
	/*0x02BC*/	char _unk02BC[0x158];
	/*0x0414*/	float x;
	/*0x0418*/	float y;
	/*0x041C*/	char _unk041C[0x350];
} ENTITY, PENTITY; /*0x076C*/

typedef struct _Player : _Entity
{
	/*0x076C*/	char _unk76C[0x3E4];
	/*0x0B50*/	int _maxHearts;
	/*0x0B54*/	int _hearts;
	/*0x0B58*/	int _eternalHearts;
	/*0x0B5C*/	int _soulHearts;
	/*0x0B60*/	int _blackHeartMask;
	/*0x0B64*/	int _jarHearts;
	/*0x0B68*/	int _numKeys;
	/*0x0B6C*/	int _hasGoldenKey;
	/*0x0B70*/	int _numBombs;
	/*0x0B74*/	int _numCoins;
	/*0x0B78*/	char _unk0B78[0x8];
	/*0x0B80*/	int _attackingOrMoving;
	/*0x0B84*/	char _unk0B84[0x18];
	/*0x0B9C*/	char* _characterName;
	/*0x0BA0*/	char _unk0BA0[0x40];
	/*0x0BE0*/	int _tearrate;
	/*0x0BE4*/	float _shotspeed;
	/*0x0BE8*/	char _unk0BE8[0x4];
	/*0x0BEC*/	int _ntearsfired;
	/*0x0BF0*/	float _damage;
	/*0x0BF4*/	char _unk0BF4[0xB8];
	/*0x0CAC*/	float _speed;
	/*0x0CB0*/	float _luck;
	/*0x0CB4*/	char _unk0CC4[0x18];
	/*0x0CCC*/	int _charges;
	/*0x0CD0*/	char _unk0CD0[0x94];
	/*0x0D64*/	int _trinket1ID;
	/*0x0D68*/	int _trinket2ID;
	/*0x0D6C*/	char _unk0D6C[0x4];
	/*0x0D70*/	int _items[0x15A];
	/*0x12D8*/	char _unk12D8[0x1CBC];
	/*0x2F94*/	int _pocket1ID;
	/*0x2F98*/	int _pocket1isCard;
	/*0x2F9C*/	int _pocket2ID;
	/*0x2FA0*/	int _pocket2isCard;
} PLAYER, *PPLAYER;

typedef struct _PlayerManager
{
	int _floorNo;
	bool _alternateFloor;
	char unknown1[0x03];
	int _curses;
	char unknown2[0x01];
	bool _seeForever;
	char unknown3[0x0A];
	//Room rooms[50]; // unknown size.. 50 for now
	char unknown4[0x3BB8];
	int RoomCount;
	char unknown5[0x23D1];
	bool unknown6;
	// f23936 = floor Seed
	// f23920 = current seed
} PLAYERMANAGER, *PPLAYERMANAGER;

GLOBAL bool gbAttached;

GLOBAL PPLAYER gpPlayer;
GLOBAL PPLAYER GetPlayer();
GLOBAL PPLAYERMANAGER gpPlayerManager;
GLOBAL PPLAYERMANAGER GetPlayerManager();

#endif
