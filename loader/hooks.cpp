#include "hooks.h"
#include "utilities.h"
#include "plugins.h"
#include "statics.h"

DWORD gdwBaseAddress = (DWORD)GetModuleHandle(NULL);
DWORD gdwBaseSize = (DWORD)dwGetModuleSize("isaac-ng.exe");

/* who knows why this is here? */
void __stdcall nullstub()
{
}

DWORD gdwStartGame = 0;
//int(__fastcall *original_startGame)(int challenge_id, bool disable_achievements, int character_id, basic_string<char, char_traits<char>, allocator<char>> seed, bool hard);
int(__fastcall *original_startGame)(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int seed_length, unsigned int max_length, int a10, bool hard);
int __fastcall startGame(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int seed_length, unsigned int max_length, int a10, bool hard)
{
	int ret = original_startGame(challenge_id, disable_achievements, character_id, a4, a5, a6, a7, seed_length, max_length, a10, hard);
	PostStartGame(ret);
	return ret;
}

/*
addCollectible is called when you pick up a pedestal, shop, market, or devil/angel item
it includes active(spacebar), passive, and familiar(followers)
:params pPlayer: a pointer to our player entity
:params relatedID: relatedID != itemID if you start with the item, otherwise relatedID and itemID are the same for pickups
:params charges: number of charges on the item currently (0-6 for standard items, 0x100+ for items that recharge in room)
:params arg5: currently unknown what this is for (maybe a boolean value? i have only seen 1 or 0
*/
DWORD gdwAddCollectible = 0;
int(__fastcall *original_addCollectible)(Player *pPlayer, int relatedID, int itemID, int charges, int arg5);
int __fastcall addCollectible(Player *pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	int	ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	PostAddCollectible(ret);
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
int(__fastcall *original_Level__Init)(Player *pPlayer, int _EDX);
int __fastcall Level__Init(Player *pPlayer, int _EDX)
{
	int	ret = original_Level__Init(pPlayer, _EDX);
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
	}
	while (!PostItemPool__GetCollectible(ret));
	return ret;
}

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
}

/* GameUpdate Function */
DWORD gdwGameUpdate = 0;
void(__cdecl *original_gameUpdate)();
void __cdecl gameUpdate()
{
	OnGameUpdate();
	original_gameUpdate();
}


#define printarg(i, arg) cout << i << ":" << "\t" << (void *)arg << "\t" << (float)arg << endl;
DWORD gdw576270 = 0;
bool(__fastcall *original_576270)(int a1, int _EDX, int a2, int a3, int a4);
bool __fastcall sub_576270(int a1, int _EDX, int a2, int a3, int a4)
{
	/*
	if (!*(DWORD *)(a1 + 0x10))
	{
		DWORD v2 = *(DWORD *)(a1 + 0xCEC);
	}
	cout << "576270: " << (void *) << endl;
	*/

	bool rTriggerHeld = false;
	bool ret = original_576270(a1, _EDX, a2, a3, a4);
	if (a2 == 11 && rTriggerHeld && ret)
		cout << "Dropping Trinkets and PocketItems" << endl;
	if (ret) {
		//printarg("a2", a2);
	}
	return ret;
}

void InitHooks()
{
	/* scan for functions */
	gdwGameUpdate = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x51\x53\x56\x8b\x35\x00\x00\x00\x00\x57",
		"xxxxxxxx????x");
	if (gdwGameUpdate)
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)gdwGameUpdate, (PBYTE)gameUpdate);

	gdwStartGame = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00"
		"\x50\x83\xEC\x08\x53\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\xF4"
		"\x64\xA3\x00\x00\x00\x00\x8B\x45\x28",
		"xxxxxx????xx????xxxxxxxx????xxxxxxxx????xxx");
	if (gdwStartGame)
		original_startGame = (int(__fastcall *)(int challenge_id, bool disable_achievements, int character_id, int a4, int a5, int a6, int a7, int a8, unsigned int a9, int a10, bool hard))DetourFunction((PBYTE)gdwStartGame, (PBYTE)startGame);

	gdwAddCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");
	if (gdwAddCollectible)
		original_addCollectible = (int(__fastcall *)(Player *, int, int, int, int))DetourFunction((PBYTE)gdwAddCollectible, (PBYTE)addCollectible);

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
		original_Level__Init = (int(__fastcall *)(Player *, int))DetourFunction((PBYTE)dwLevel__Init, (PBYTE)Level__Init);

	dwItemPool__GetCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xEC\x28\x8B\x45\x08","xxxxxxxxx");
	if (dwItemPool__GetCollectible)
		original_ItemPool__GetCollectible = (int(__fastcall *)(int, int, int, int, int))DetourFunction((PBYTE)dwItemPool__GetCollectible, (PBYTE)ItemPool__GetCollectible);

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
	*/

	/*
	gdw576270 = gdwBaseAddress + 0x176270;
		/*
		dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x8B\x45\x0C\x53\x32\xDB\x56\x8B"
		"\xF1\x83\xF8\xFF\x75\x7D\x8B\x46\x38\x8B\x50\x08\x8D"
		"\x4E\x38\x57\x6A\xFF\x89\x4D\x0C\xFF\xD2\x8B\x46\x08"
		"\x2B\x46\x04\x33\xFF\xC1\xF8\x02\x85\xC0\x74\x4A\x8B"
		"\x4E\x04\x8B\x0C\xB9\x8B\x11\x8B\x45\x08\x8B\x52\x20",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if (gdw576270)
	{
		cout << "gdw576270: " << (void *)(gdw576270 - gdwBaseAddress) << endl;
		original_576270 = (bool(__fastcall *)(int a1, int _EDX, int a2, int a3, int a4))DetourFunction((PBYTE)gdw576270, (PBYTE)sub_576270);
	}*/
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)original_gameUpdate, (PBYTE)gameUpdate);
	DetourRemove((PBYTE)original_addCollectible, (PBYTE)addCollectible);
}
