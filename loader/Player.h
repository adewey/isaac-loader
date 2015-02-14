#ifndef _Player_H_
#define _Player_H_

class Entity
{
public:
	/*[0x0000]*/	//void * vftable;
	/*[0x0004]*/	char _unk0000[0x8];
	/*[0x000C]*/	int _id;
	/*[0x0010]*/	unsigned int _variant;
	/*[0x0014]*/	int _subtype;
	/*[0x0018]*/	int _parentID;
	/*[0x001C]*/	unsigned int _parentVariant;
	/*[0x0020]*/	int _parentSubtype;
	/*[0x0024]*/	char _unk0024[0x50];
	/*[0x0074]*/	int _tearType;
	/*[0x0078]*/	char _unk0078[0x23C];
	/*[0x02B4]*/	float _scaleX;
	/*[0x02B8]*/	float _scaleY;
	/*[0x02BC]*/	char _unk02BC[0x52];
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
	/*[0x0000]*/	virtual void *Player_VF_0x000(bool a2); //
}; /*0x076C*/

class Player : Entity
{
public:
	/*[0x076C]*/		char _unk76C[0x3E4];
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
	/*[0x0B78]*/		char _unk0B78[0x8];
	/*[0x0B80]*/		int _attackingOrMoving;
	/*[0x0B84]*/		char _unk0B84[0x14];
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
	/*[0x0CBC]*/		char _unk0CBC[0x10];
	/*[0x0CCC]*/		int _charges;
	/*[0x0CD0]*/		char _unk0CD0[0x94];
	/*[0x0D64]*/		int _trinket1ID;
	/*[0x0D68]*/		int _trinket2ID;
	/*[0x0D6C]*/		char _unk0D6C[0x4];
	/*[0x0D70]*/		int _items[0x15A];
	/*[0x12D8]*/		char _unk12D8[0x1CBC];
	/*[0x2F94]*/		int _pocket1ID;
	/*[0x2F98]*/		bool _pocket1isCard;
	/*[0x2F99]*/		char unk2F99[0x3]; //fix alignment
	/*[0x2F9C]*/		int _pocket2ID;
	/*[0x2FA0]*/		bool _pocket2isCard;
	/*[0x2FA1]*/		char _unk2FA1[0x3]; //fix alignment
	/*[0x2FA4]*/		char _unk2FA4[0x10];
	/*[0x2FB4]*/		int _nGuppyItems;
	/*[0x2FB8]*/		int _nFlyItems;
	/*[0x0000]*/	virtual void *Player_VF_0x000(bool a2); //
	/*[0x0004]*/	virtual int Player_VF_0x004(int a2, int Args, unsigned int a4, int a5); //
	/*[0x0008]*/	virtual int Player_VF_0x008(); //poossibly playerupdate
	/*[0x000C]*/	virtual int Player_VF_0x00C(); //related to player move
	/*[0x0010]*/	virtual int Player_VF_0x010(int a2); //
	/*[0x0014]*/	virtual bool Player_VF_0x014(int a2); //draw shadow?
	/*[0x0018]*/	virtual bool Player_VF_0x018(float a2, int a3, float a4, int a5); //
	/*[0x001C]*/	virtual void Player_VF_0x01C(); //
	/*[0x0020]*/	virtual void Player_VF_0x020(); //this->_unk030E = false; if (this->_unk030F == false) this->_unk030F = true;
	/*[0x0024]*/	virtual void Player_VF_0x024(); //do something with position? collision?
	/*[0x0028]*/	virtual int Player_VF_0x028(int a2, int a3, int a4, int a5, int a6); //jumptable
	/*[0x002C]*/	virtual void Player_VF_0x02C(float a2); //this->_unk0x0458 = a2
	/*[0x0030]*/	virtual int Player_VF_0x030(); //
	/*[0x0034]*/	virtual bool Player_VF_0x034(); //return 0;
	/*[0x0038]*/	virtual bool Player_VF_0x038(); //return 0; same pointer as Player_VF_0x034 // stub functions?
	/*[0x003C]*/	virtual bool Player_VF_0x03C(int a2, int a3); //a2 = pointer of some kind
	/*[0x0040]*/	virtual int Player_VF_0x040(); //
	/*[0x0044]*/	virtual int Player_VF_0x044(); // collision related
};

#endif /*_Player_H_*/
