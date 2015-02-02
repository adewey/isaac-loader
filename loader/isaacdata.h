#ifndef ISAACDATA_H
#define ISAACDATA_H

struct Entity
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
}; /*0x076C*/

struct Player : Entity
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
	/*0x0B78*/	char _unk0B78[0x24];
	/*0x0B9C*/	char* _characterName;
	/*0x0BA0*/	char _unk0BA0[0x3C];
	/*0x0BDC*/	float _shotspeed;
	/*0x0BE0*/	char _unk0BEO[0x08];
	/*0x0BE8*/	float _damage;
	/*0x0BEC*/	float _range;
	/*0x0BF0*/	char _unk0BF0[0xBC];
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
};





#endif
