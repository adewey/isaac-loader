#ifndef _Resource_H_
#define _Resource_H_

#include "Item.h"

class Resource
{
public:
/*0x00000*/ char unk0x00000[0xDD30];
/*0x0DD30*/ char unk0x0DD30[0x4]; // very possible array of pointers for something
/*0x0DD34*/ char unk0x0DD34[0x21784];
/*0x2F4B8*/ int unk0x2F4B8; // non zero
/*0x2F4BC*/ char unk0x2F4BC[0x4]; // .text:004AC07F                 cmp     byte ptr [eax+2F4BEh], 0 ; possibly related to settings and the graphics filter
/*0x2F4C0*/ Item *ItemList[0x15A]; // 0x568
/*0x2FA28*/ char unk0x2FA28[0x514];
/*0x2FF3C*/ int unk0x2FF3C;
};

#endif /*_Resource_H_*/