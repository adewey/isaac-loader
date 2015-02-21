#ifndef _Item_H_
#define _Item_H_


class Item
{
public:
	/*0x00*/	int _state;
	/*0x04*/	int _id;
	/*0x08*/	union {
		char *_pszName;
		char _szName[0x1C];
	};
	/*0x24*/	union {
		char *_pszDescription;
		char _szDescription[0x1C];
	};
	/*0x40*/	union {
		char *_pszResourcePath;
		char _szResourcePath[0x18];
	};
	/*0x58*/	int _gfx;
	/*0x5C*/	int _achievement;
	/*0x60*/	int _cache; // > 0 if locked?
	/*0x64*/	int _maxhearts;
	/*0x68*/	int _hearts;
	/*0x6C*/	int _soulhearts;
	/*0x70*/	int _blackhearts;
	/*0x74*/	int _bombs;
	/*0x78*/	int _keys;
	/*0x7C*/	int _coins;
	/*0x80*/	int _maxcharges;
	/*0x84*/	int _cooldown;
	/*0x88*/	int _special;
	/*0x8C*/	int _devilprice;
	/*0x90*/	int unknown0x90; // 5a locked ff
	/*0x94*/	union {
		char *_pszAnimationPath;
		char _szAnimationPath[0x1C];
	};
	/*0xB0*/	int unknown0xB0;
	/*0xB0*/	int unknown0xB4;
}; /*0xC8*/

#endif /*_Item_H_*/
