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
		call nullstub
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

/* add other global hooks here, and expose them for our events to catch */
DWORD gdwPlayerManager = 0;
DWORD gdwGetPlayerEntity = 0;

void InitHooks()
{
	/* scan for functions */
	gdwGameUpdate = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x51\x53\x56\x8b\x35\x00\x00\x00\x00\x57",
		"xxxxxxxx????x");

	if (gdwGameUpdate)
	{
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)gdwGameUpdate, (PBYTE)gameUpdate);
	}

	gdwAddCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");

	if (gdwAddCollectible)
	{
		original_addCollectible = (int(__fastcall *)(Player *, int, int, int, int))DetourFunction((PBYTE)gdwAddCollectible, (PBYTE)addCollectible);
	}

	gdwPlayerManager = *(DWORD*)(dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x8b\x0d\x00\x00\x00\x00\x8b\x81\x30\x8a\x00\x00\x2b\x81\x2c\x8a\x00\x00\xc1\xf8\x02\xc3",
		"xx????xx????xx????xxxx") + 2);

	dwChangeKeys = gdwBaseAddress + 0xCEBE0;

	if (dwChangeKeys)
	{
		original_changeKeys = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwChangeKeys, (PBYTE)changeKeys);
	}

	dwChangeBombs = gdwBaseAddress + 0xCEB90;

	if (dwChangeBombs)
	{
		original_changeBombs = (int(__fastcall *)(Player *, int, int))DetourFunction((PBYTE)dwChangeBombs, (PBYTE)changeBombs);
	}

	dwChangeCoins = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x0C\x53\x8B\xD8",
		"xxxxxxxxxxxx");// gdwBaseAddress + 0xCEAB0;

	if (dwChangeCoins)
	{
		original_changeCoins = (DWORD)DetourFunction((PBYTE)dwChangeCoins, (PBYTE)changeCoins);
	}

	gdwGetPlayerEntity = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x8B\x86\x00\x00\x00\x00\x2B\x86\x00\x00\x00\x00\xA9\x00\x00\x00\x00\x75\x0F\x68\x00"
		"\x00\x00\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x08\x8B\x8E\x00\x00\x00\x00\x2B\x8E"
		"\x00\x00\x00\x00\xC1\xF9\x02\x3B\xF9\x73\x0C",
		"xx????xx????x????xxx????xxx????xxxxx????xx????xxxxxxx");//gdwBaseAddress + 0xFA50;
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
