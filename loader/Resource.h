#ifndef _Resource_H_
#define _Resource_H_

#include "Item.h"

class SavedGame
{
public:
/*0x000*/	char unk0x000[0x9C];
/*0x094*/	int _character_id;
/*0x098*/	int _challenge_id;
/*0x09C*/	char saved_seed[0x10];
/*0x0A0*/	int saved_seed_len;
/*0x0A4*/	char unk0x0A4[0x14];
/*0x0B8*/	BOOL hard_mode;
/*0x0BC*/	BOOL _disable_achievements;
/*0x0B0*/	char unk0x0B0[0xA0];
}; /*0x150*/


class Resource
{
public:
/*0x00000*/ char unk0x00000[0xDD30];
/*0x0DD30*/ char unk0x0DD30[0x4]; // very possible array of pointers for something
/*0x0DD34*/ char unk0x0DD34[0x21784];
/*0x2F4B8*/ int unk0x2F4B8; // non zero
/*0x2F4BC*/ char unk0x2F4BC[0x4]; // .text:004AC07F                 cmp     byte ptr [eax+2F4BEh], 0 ; possibly related to settings and the graphics filter
/*0x2F4C0*/ Item *ItemList[0x15A]; // 0x568
/*0x2FA28*/ char unk0x2FA28[0x514]; //v3 = *(_DWORD *)(LODWORD(dwResourceManager) + 4 * a2 + 0x2FA2C); trinket list?
/*0x2FF3C*/ int unk0x2FF3C;
/*0x2FF40*/ char unk0x2FF40[0x274];
/*0x301B4*/ SavedGame saved_game; 
};

#endif /*_Resource_H_*/