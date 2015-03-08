#ifndef _Player_H_
#define _Player_H_

class Entity
{
public:
/*[0x0000]*/	void *vftable;
/*[0x0004]*/	char _unk0000[0x8];
/*[0x000C]*/	int _type;
/*[0x0010]*/	unsigned int _variant;
/*[0x0014]*/	int _id;
/*[0x0018]*/	int _parentID;
/*[0x001C]*/	unsigned int _parentVariant;
/*[0x0020]*/	int _parentSubtype;
/*[0x0024]*/	char _unk0024[0x50];
/*[0x0074]*/	int _tearType;
/*[0x0078]*/	char _unk0078[0x23C];
/*[0x02B4]*/	float _scaleX;
/*[0x02B8]*/	float _scaleY;
/*[0x02BC]*/	char _unk02BC[0x4C];
/*[0x0308]*/	DWORD keymap; // >> 14 bomb key?
/*[0x030C]*/	char _unk030C[2];
/*[0x030E]*/	bool _unk030E;
/*[0x030F]*/	bool _unk030F;
/*[0x0310]*/	char _unk0310[0xE4];
/*[0x03F4]*/	int _statusEffectDuration;
/*[0x03F8]*/	char _unk03F8[0x1C];
/*[0x0414]*/	float _x;
/*[0x0418]*/	float _y;
/*[0x041C]*/	char _unk041C[0x3C];
/*[0x0458]*/	float _unk0x0458;
/*[0x045C]*/	char _unk0x045C[0x310];
}; /*0x076C*/

class Player : Entity
{
public:
/*[0x076C]*/		char _unk76C[0x4]; // triggerbomb: cmp     byte ptr [esi+76Ch], 0
/*[0x0770]*/		int _freezeFrames; // setting this > 0 will freeze the character in place until x frames have passed. _freezeFrames--; every frame.
/*[0x0774]*/		char _unk774[0x3DC];
/*[0x0B50]*/		int _maxHearts;
/*[0x0B54]*/		int _hearts;
/*[0x0B58]*/		int _eternalHearts;
/*[0x0B5C]*/		int _soulHearts;
/*[0x0B60]*/		int _blackHeartMask;
/*[0x0B64]*/		int _jarHearts;
/*[0x0B68]*/		int _numKeys;
/*[0x0B6C]*/		int _hasGoldenKey;
/*[0x0B70]*/		int _numBombs;
/*[0x0B74]*/		int _numCoins;
/*[0x0B78]*/		char _unk0B78[0xC];
/*[0x0B84]*/		int _attackingOrMoving;
/*[0x0B8C]*/		char _unk0B8C[0x10];
/*[0x0B98]*/		int _charID;
/*[0x0B9C]*/		char* _characterName;
/*[0x0BA0]*/		char _unk0BA0[0x40];
/*[0x0BE0]*/		int _tearrate;
/*[0x0BE4]*/		float _shotspeed;
/*[0x0BE8]*/		char _unk0BE8[0x4];
/*[0x0BEC]*/		int _ntearsfired;
/*[0x0BF0]*/		float _damage;
/*[0x0BF4]*/		char _unk0BF4[0x4];
/*[0x0BF8]*/		float _range;
/*[0x0BFC]*/		char _unk0BFC[0xB8];
/*[0x0CB4]*/		float _speed;
/*[0x0CB8]*/		float _luck;
/*[0x0CBC]*/		char _unk0CBC[0xC];
/*[0x0CC8]*/		int _helditemid;
/*[0x0CCC]*/		int _charges;
/*[0x0CD0]*/		char _unk0CD0[0x94]; // CE4 <= 0 bomb cooldown time
										// CF4 facing .. 0 = left, 1 = up, 2 = right, 3 = down
										// CD8 && CD4 != 0 spacebar press
/*[0x0D64]*/		int _trinket1ID;
/*[0x0D68]*/		int _trinket2ID;
/*[0x0D6C]*/		char _unk0D6C[0x4];
/*[0x0D70]*/		int _items[0x15A];
/*[0x12D8]*/		char _unk12D8[0x1CBC];
/*[0x2F94]*/		int _pocket1ID;
/*[0x2F98]*/		BOOL _pocket1isCard;
/*[0x2F9C]*/		int _pocket2ID;
/*[0x2FA0]*/		BOOL _pocket2isCard;
/*[0x2FA4]*/		char _unk2FA4[0x10];
/*[0x2FB4]*/		int _nGuppyItems;
/*[0x2FB8]*/		int _nFlyItems;
};

#endif /*_Player_H_*/
