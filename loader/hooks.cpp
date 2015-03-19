#include "hooks.h"
#include "utilities.h"
#include "plugins.h"
#include "statics.h"
#define printarg(i, arg) cout << i << ":" << "\t" << (void *)arg << "\t" << (float)arg << endl;

DWORD gdwBaseAddress = (DWORD)GetModuleHandle(NULL);
DWORD gdwBaseSize = (DWORD)dwGetModuleSize("isaac-ng.exe");

/* who knows why this is here? */
void __stdcall nullstub()
{
}

DWORD gdwGame__Start = 0;
//int(__fastcall *original_Game__Start)(int challenge_id, bool disable_achievements, int character_id, basic_string<char, char_traits<char>, allocator<char>> seed, bool hard);
int(__fastcall *original_Game__Start)(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int seed_length, unsigned int max_length, int a10, bool hard_mode);
int __fastcall Game__Start(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int seed_length, unsigned int max_length, int a10, bool hard_mode)
{
	PreGame__Start(&character_id, &disable_achievements, &character_id, (char *)&a4, &hard_mode);
	OnGame__Start(challenge_id, disable_achievements, character_id, (char *)&a4, hard_mode);
	int ret = original_Game__Start(challenge_id, disable_achievements, character_id, a4, a5, a6, a7, seed_length, max_length, a10, hard_mode);
	PostGame__Start(ret);
	return ret;
}

/*
Player_Entity__AddCollectible is called when you pick up a pedestal, shop, market, or devil/angel item
it includes active(spacebar), passive, and familiar(followers)
:params pPlayer: a pointer to our player entity
:params relatedID: relatedID != itemID if you start with the item, otherwise relatedID and itemID are the same for pickups
:params charges: number of charges on the item currently (0-6 for standard items, 0x100+ for items that recharge in room)
:params arg5: currently unknown what this is for (maybe a boolean value? i have only seen 1 or 0
*/
DWORD gdwPlayer_Entity__AddCollectible = 0;
int(__fastcall *original_Player_Entity__AddCollectible)(Player *pPlayer, int _EDX, int item_id, int charges, int arg5);
int __fastcall Player_Entity__AddCollectible(Player *pPlayer, int _EDX, int item_id, int charges, int arg5)
{
	PrePlayer_Entity__AddCollectible(pPlayer, &item_id, &charges);
	OnPlayer_Entity__AddCollectible(pPlayer, item_id, charges);
	int	ret = original_Player_Entity__AddCollectible(pPlayer, _EDX, item_id, charges, arg5);
	PostPlayer_Entity__AddCollectible(ret);
	return ret;
}


DWORD dwTriggerBossDeath = 0;
DWORD original_TriggerBossDeath = 0;
__declspec(naked) char TriggerBossDeath()
{
	_asm
	{
#ifndef _DEBUG
		call nullstub
#endif
			jmp original_TriggerBossDeath
			push eax
			push eax
			call PostTriggerBossDeath
			add esp, 4
			pop eax
	}
}



DWORD dwLevel__Init = 0;
int(__fastcall *original_Level__Init)(int level, int _EDX);
int __fastcall Level__Init(int level, int _EDX)
{
	OnLevel__Init(level);
	int	ret = original_Level__Init(level, _EDX);
	PostLevel__Init(ret);
	return ret;
}

DWORD dwEntity_Pickup__Init = 0;
int(__fastcall *original_Entity_Pickup__Init)(Entity *pItem, int _EDX, int type, int variant, int subtype, int unkown);
int __fastcall Entity_Pickup__Init(Entity *pItem, int _EDX, int type, int variant, int subtype, int unkown)
{
	DWORD ret = original_Entity_Pickup__Init(pItem, _EDX, type, variant, subtype, unkown);
	//while (pedistalitem && !PostEntity_Pickup__Init(pItem->_id)); // make sure we are a pedistal item before rerolling this. we could also use this function to reroll other ground items..
	return ret;
}

DWORD dwItemPool__GetCollectible = 0;
int(__fastcall *original_ItemPool__GetCollectible)(int itempool, int _EDX, int pool_index, int always1, int seed);
int __fastcall ItemPool__GetCollectible(int itempool, int _EDX, int pool_index, int always1, int seed)
{
	DWORD ret = 0;
	do
	{
		ret = original_ItemPool__GetCollectible(itempool, _EDX, pool_index, always1, seed);
	} while (!PreItemPool__GetCollectible(ret));
	PostItemPool__GetCollectible(ret);
	return ret;
}

DWORD dwItemPool__GetCard = 0;
int(__fastcall *original_ItemPool__GetCard)(int itempool, int _EDX, bool a2, bool get_rune);
int __fastcall ItemPool__GetCard(int itempool, int _EDX, bool a2, bool get_rune)
{
	DWORD ret = 0;
	if (get_rune)
	{
		do
		{
			ret = original_ItemPool__GetCard(itempool, _EDX, a2, get_rune);
		} while (!PreItemPool__GetRune(ret));
		PostItemPool__GetRune(ret);
	}
	else {
		do
		{
			ret = original_ItemPool__GetCard(itempool, _EDX, a2, get_rune);
		} while (!PreItemPool__GetCard(ret));
		PostItemPool__GetCard(ret);
	}
	return ret;
}

/*
DWORD dwEntity_Pickup__Morph = 0;
DWORD original_Entity_Pickup__Morph = 0;
__declspec(naked) char Entity_Pickup__Morph()
{
	_asm
	{
		push ebp
			mov ebp, esp
			push ebx
			push dword ptr[ebp + 0x14]
			push dword ptr[ebp + 0x10]
			push dword ptr[ebp + 0x0C]
			push dword ptr[ebp + 0x08]
			push ebx
			call OnEntity_Pickup__Morph
			add esp, 20
		pop ebx
		pop ebp
		jmp original_Entity_Pickup__Morph
	}
}*/



DWORD dw_sub_5302D0 = 0;
int(__fastcall *o_sub_5302D0)(int a1, int _EDX, int a2, int a3, int a4, int a5, int a6, int a7, unsigned int a8, int a9, int a10, char a11);
int __fastcall sub_5302D0(int a1, int _EDX, int a2, int a3, int a4, int a5, int a6, int a7, unsigned int a8, int a9, int a10, char a11)
{
	/*
	printarg("a1", a1);
	printarg("a2", a2);
	printarg("a3", a3);
	cout << "a3*:\t" << (char *)&a3 << endl;
	strcpy((char *)&a3, "TEST SEED");
	printarg("a4", a4);
	printarg("a5", a5);
	printarg("a6", a6);
	printarg("a7", a7);
	printarg("a8", a8);
	printarg("a9", a9);
	printarg("a10", a10);
	printarg("a11", a11);
	*/
	int ret = o_sub_5302D0(a1, _EDX, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
	/*
	printarg("ret", ret);
	*/
	return ret;
}


/* GameUpdate Function */
DWORD gdwGameUpdate = 0;
void(__cdecl *original_gameUpdate)();
void __cdecl gameUpdate()
{
	OnGameUpdate();
	original_gameUpdate();
}

void InitHooks()
{
	/* scan for functions */
	gdwGameUpdate = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x51\x53\x56\x8b\x35\x00\x00\x00\x00\x57",
		"xxxxxxxx????x");
	if (gdwGameUpdate)
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)gdwGameUpdate, (PBYTE)gameUpdate);

	gdwGame__Start = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00"
		"\x50\x83\xEC\x08\x53\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\xF4"
		"\x64\xA3\x00\x00\x00\x00\x8B\x45\x28",
		"xxxxxx????xx????xxxxxxxx????xxxxxxxx????xxx");
	if (gdwGame__Start)
		original_Game__Start = (int(__fastcall *)(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int a8, unsigned int a9, int a10, bool hard))DetourFunction((PBYTE)gdwGame__Start, (PBYTE)Game__Start);

	gdwPlayer_Entity__AddCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");
	if (gdwPlayer_Entity__AddCollectible)
		original_Player_Entity__AddCollectible = (int(__fastcall *)(Player *, int, int, int, int))DetourFunction((PBYTE)gdwPlayer_Entity__AddCollectible, (PBYTE)Player_Entity__AddCollectible);

	dwTriggerBossDeath = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x80\x7E\x01\x00\x0F\x85",
		"xxxxxx");
	if (dwTriggerBossDeath)
		original_TriggerBossDeath = (DWORD)DetourFunction((PBYTE)dwTriggerBossDeath, (PBYTE)TriggerBossDeath);

	dwLevel__Init = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1"
		"\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00\xA1\x00\x00"
		"\x00\x00\x33\xC5\x89\x45\xF0\x53\x56\x57\x50\x8D\x45\xF4"
		"\x64\xA3\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\xF1",
		"xxxxxx????xx????xxx????x????xxxxxxxxxxxxxx????x????xx");
	if (dwLevel__Init)
		original_Level__Init = (int(__fastcall *)(int, int))DetourFunction((PBYTE)dwLevel__Init, (PBYTE)Level__Init);

	dwItemPool__GetCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xEC\x28\x8B\x45\x08", "xxxxxxxxx");
	if (dwItemPool__GetCollectible)
		original_ItemPool__GetCollectible = (int(__fastcall *)(int, int, int, int, int))DetourFunction((PBYTE)dwItemPool__GetCollectible, (PBYTE)ItemPool__GetCollectible);

	dwItemPool__GetCard = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xA1\x00\x00"
		"\x00\x00\x33\xC5\x89\x45\xFC\x53\x56\x57\x68\x00\x00\x00"
		"\x00\x6A\x01",
		"xxxxx????x????xxxxxxxxx????xx");
	if (dwItemPool__GetCard)
		original_ItemPool__GetCard = (int(__fastcall *)(int, int, bool, bool))DetourFunction((PBYTE)dwItemPool__GetCard, (PBYTE)ItemPool__GetCard);

	/*
	dwEntity_Pickup__Init = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00"
		"\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00"
		"\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x78",
		"xxxxxxxxx????xx????xxx????x????xxxxxx");
	if (dwEntity_Pickup__Init)
		original_Entity_Pickup__Init = (int(__fastcall *)(Entity *, int, int, int, int, int))DetourFunction((PBYTE)dwEntity_Pickup__Init, (PBYTE)Entity_Pickup__Init);

	dwEntity_Pickup__Morph = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1"
		"\x00\x00\x00\x00\x50\x83\xEC\x78\xA1\x00\x00\x00\x00\x33"
		"\xC5\x89\x45\xF0\x56",
		"xxxxxx????xx????xxxxx????xxxxxx");
	if (dwEntity_Pickup__Morph)
		original_Entity_Pickup__Morph = (DWORD)DetourFunction((PBYTE)dwEntity_Pickup__Morph, (PBYTE)Entity_Pickup__Morph);

	dw_sub_5302D0 = gdwBaseAddress + 0x1302D0;
	if (dw_sub_5302D0)
		o_sub_5302D0 = (int(__fastcall *)(int a1, int _EDX, int a2, int a3, int a4, int a5, int a6, int a7, unsigned int a8, int a9, int a10, char a11))DetourFunction((PBYTE)dw_sub_5302D0, (PBYTE)sub_5302D0);

		*/

}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)original_gameUpdate, (PBYTE)gameUpdate);
	DetourRemove((PBYTE)original_Player_Entity__AddCollectible, (PBYTE)Player_Entity__AddCollectible);
}
