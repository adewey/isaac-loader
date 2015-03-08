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

DWORD dwAddKeys = 0;
int(__fastcall *original_AddKeys)(Player *pPlayer, int _EDX, int nKeys);
int __fastcall AddKeys(Player *pPlayer, int _EDX, int nKeys)
{
	int	ret = original_AddKeys(pPlayer, _EDX, nKeys);
	PostAddKeys(ret);
	return ret;
}

DWORD dwAddBombs = 0;
int(__fastcall *original_AddBombs)(Player *pPlayer, int _EDX, int nBombs);
int __fastcall AddBombs(Player *pPlayer, int _EDX, int nBombs)
{
	int	ret = original_AddBombs(pPlayer, _EDX, nBombs);
	PostAddBombs(ret);
	return ret;
}

DWORD dwAddCoins = 0;
DWORD original_AddCoins = 0;
__declspec(naked) char AddCoins()
{
	_asm
	{
#ifndef _DEBUG
		call nullstub
#endif
		jmp original_AddCoins
			push eax
			push eax
			call PostAddCoins
			add esp, 4
		pop eax
	}
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

#define printarg(i, arg) cout << i << ":" << "\t" << (void *)arg << "\t" << (float)arg << endl;
DWORD dwEntity_Pickup__Init = 0;
int(__fastcall *original_Entity_Pickup__Init)(Entity *pItem, int _EDX, int type, int variant, int subtype, int unkown);
int __fastcall Entity_Pickup__Init(Entity *pItem, int _EDX, int type, int variant, int subtype, int unkown)
{
	printarg("pItem", (int)pItem);
	printarg("type", type);
	printarg("variant", variant);
	printarg("subtype", subtype);
	printarg("unkown", unkown);
	DWORD ret = 0;

	bool shopitem = type == 0x05 && variant == 0x64 && subtype != 0x00;
	bool pedistalitem = type == 0x05 && variant == 0x64 && subtype == 0x00;
	/*
	if (shopitem)
	{
		do
			ret = original_Entity_Pickup__Init(pItem, _EDX, type, variant, subtype, unkown);
		while (!PostEntity_Shop_Pickup__Init(subtype)); // make sure we are a pedistal item before rerolling this. we could also use this function to reroll other ground items..
	}
	else */
	do
		ret = original_Entity_Pickup__Init(pItem, _EDX, type, variant, subtype, unkown);
	while (pedistalitem && !PostEntity_Pickup__Init(pItem->_id)); // make sure we are a pedistal item before rerolling this. we could also use this function to reroll other ground items..
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

	dwAddKeys = gdwBaseAddress + 0xCEBE0;
	if (dwAddKeys)
		original_AddKeys = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwAddKeys, (PBYTE)AddKeys);

	dwAddBombs = gdwBaseAddress + 0xCEB90;
	if (dwAddBombs)
		original_AddBombs = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwAddBombs, (PBYTE)AddBombs);

	dwAddCoins = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x0C\x53\x8B\xD8",
		"xxxxxxxxxxxx");
	if (dwAddCoins)
		original_AddCoins = (DWORD)DetourFunction((PBYTE)dwAddCoins, (PBYTE)AddCoins);

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
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)original_AddKeys, (PBYTE)addCollectible);
	DetourRemove((PBYTE)original_gameUpdate, (PBYTE)gameUpdate);
	DetourRemove((PBYTE)original_addCollectible, (PBYTE)AddKeys);
	DetourRemove((PBYTE)original_AddBombs, (PBYTE)AddBombs);
	DetourRemove((PBYTE)original_AddCoins, (PBYTE)AddCoins);
}
