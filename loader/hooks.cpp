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
	cout << "char* a4 " << (char *)&a4 << endl;
	cout << "a7: " << (void*)a7 << " " << a7 << endl;
	cout << "a10: " << (void*)a10 << " " << a10 << endl;

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

DWORD dwChangeKeys = 0;
int(__fastcall *original_changeKeys)(Player *pPlayer, int _EDX, int nKeys);
int __fastcall changeKeys(Player *pPlayer, int _EDX, int nKeys)
{
	int	ret = original_changeKeys(pPlayer, _EDX, nKeys);
	PostChangeKeys(ret);
	return ret;
}

DWORD dwChangeBombs = 0;
int(__fastcall *original_changeBombs)(Player *pPlayer, int _EDX, int nBombs);
int __fastcall changeBombs(Player *pPlayer, int _EDX, int nBombs)
{
	int	ret = original_changeBombs(pPlayer, _EDX, nBombs);
	PostChangeBombs(ret);
	return ret;
}

DWORD dwChangeCoins = 0;
DWORD original_changeCoins = 0;
__declspec(naked) char changeCoins()
{
	_asm
	{
#ifndef _DEBUG
		call nullstub
#endif
		jmp original_changeCoins
			push eax
			push eax
			call PostChangeCoins
			add esp, 4
		pop eax
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


DWORD dwsub_56AC00 = 0;
int(__fastcall *original_sub_56AC00)(int a1);
int __fastcall sub_56AC00(int a1)
{
	cout << (void *)a1 << endl;
	int ret = original_sub_56AC00(a1);
	cout << (void*)ret << endl;
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

	dwChangeKeys = gdwBaseAddress + 0xCEBE0;
	if (dwChangeKeys)
		original_changeKeys = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwChangeKeys, (PBYTE)changeKeys);

	dwChangeBombs = gdwBaseAddress + 0xCEB90;
	if (dwChangeBombs)
		original_changeBombs = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwChangeBombs, (PBYTE)changeBombs);

	dwChangeCoins = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x0C\x53\x8B\xD8",
		"xxxxxxxxxxxx");
	if (dwChangeCoins)
		original_changeCoins = (DWORD)DetourFunction((PBYTE)dwChangeCoins, (PBYTE)changeCoins);

	DWORD dwsub_56AC00 = gdwBaseAddress + 0x16AC00;
	if (dwsub_56AC00)
		original_sub_56AC00 = (int(__fastcall *)(int))DetourFunction((PBYTE)dwsub_56AC00, (PBYTE)sub_56AC00);
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)original_changeKeys, (PBYTE)addCollectible);
	DetourRemove((PBYTE)original_gameUpdate, (PBYTE)gameUpdate);
	DetourRemove((PBYTE)original_addCollectible, (PBYTE)changeKeys);
	DetourRemove((PBYTE)original_changeBombs, (PBYTE)changeBombs);
	DetourRemove((PBYTE)original_changeCoins, (PBYTE)changeCoins);
}
