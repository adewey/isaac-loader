#include "hooks.h"
#include "utilities.h"
#include "plugins.h"
#include "statics.h"
/*
addCollectible is called when you pick up a pedestal, shop, market, or devil/angel item
it includes active(spacebar), passive, and familiar(followers)
:params pPlayer: a pointer to our player entity
:params relatedID: relatedID != itemID if you start with the item, otherwise relatedID and itemID are the same for pickups
:params charges: number of charges on the item currently (0-6 for standard items, 0x100+ for items that recharge in room)
:params arg5: currently unknown what this is for (maybe a boolean value? i have only seen 1 or 0
*/
DWORD dwAddCollectible = 0;
int (__fastcall *original_addCollectible)(PPLAYER pPlayer, int relatedID, int itemID, int charges, int arg5);
int __fastcall addCollectible(PPLAYER pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	if (pPlayer != GetPlayer())
		gpPlayer = pPlayer;
	cout << "pPlayer [0x" << GetPlayer() << "]" << endl;
	int ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	
	//tell our plugins we added a collectible and give the entity id
	AddCollectible(pPlayer, relatedID, itemID, charges, arg5);

	return ret;
}

#define printarg(i, arg) cout << "Arg" << i << ":\t" << (void *)arg << "\t" << (float)arg << endl;

/*Clueless, mindlessly copied code.*/
int(__stdcall *original_spawnEntity)(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9);
int __stdcall spawnEntity(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9) 
{
	cout << "Args:\tHEX\t\tFLOAT" << endl;
	printarg(1, arg1);
	printarg(2, arg2);
	printarg(3, arg3);
	printarg(4, arg4);
	printarg(5, arg5);
	printarg(6, arg6);
	printarg(7, arg7);
	printarg(8, arg8);
	printarg(9, arg9);
	int ret = original_spawnEntity(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	cout << "Ret:\t" << (void *)ret << endl;
	return ret;
}


/* currently unused */
bool(__cdecl* original_checkForGoldenKey)();
bool checkForGoldenKey()
{
	cout << "checkForGoldenKey() Called" << endl;
	return true;
}


/*GameUpdate Function*/
DWORD dwGameUpdate = 0;
void(__cdecl *original_gameUpdate)();
void __cdecl GameUpdate()
{
	original_gameUpdate();
	OnGameUpdate();
}
/*GameUpdate Function End*/

/* add other global hooks here, and expose them for our events to catch */
DWORD **dwPlayerManager = 0;

void InitHooks()
{
	/* scan for functions */
	unsigned long dwPid = GetProcessId(L"isaac-ng.exe");
	unsigned long dwSize = 0;
	unsigned long dwBase = GetModuleBase(dwPid, L"isaac-ng.exe", &dwSize);
	cout << "dwBase [0x" << (void *)dwBase << "]" << endl;
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	dwGameUpdate = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8b\xec\x83\xec\x0c\x53\x8b\x1d\x00"
		"\x00\x00\x00\x56\x57\x8d\x83\x10\xb0\x02\x00\x50"
		"\x89\x5d\xf8\xe8\x00\x00\x00\x00\x8b\x83\x68\xe2"
		"\x12\x00\x85\xc0\x7e\x00\x48\x89\x83\x68\xe2\x12\x00",
		"xxxxxxxxx????xxxxxxxxxxxxx????xxxxxxxxx?xxxxxxx");

	if (dwGameUpdate)
	{
		cout << "Found dwGameUpdate: [0x" << (void *)(dwGameUpdate - dwBase) << "]" << endl;
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)dwGameUpdate, (PBYTE)GameUpdate);
	}

	dwAddCollectible = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");

	if (dwAddCollectible)
	{
		cout << "dwItemPickup found [0x" << (void *)(dwAddCollectible - dwBase) << "]" << endl;
		original_addCollectible = (int(__fastcall *)(PPLAYER, int, int, int, int))DetourFunction((PBYTE)dwAddCollectible, (PBYTE)addCollectible);
	}

	dwPlayerManager = (DWORD **)dwFindPattern(dwBase, dwSize, 
		(BYTE*)"\x8b\x0d\x00\x00\x00\x00\x8b\x81\x9c\x94"
		"\x00\x00\x2b\x81\x98\x94\x00\x00\xc1\xf8\x02\xc3",
		"xx????xxxxxxxxxxxxxxxx") + 2;

	if (dwPlayerManager)
	{
		gpPlayerManager = (PPLAYERMANAGER)dwPlayerManager;
		cout << "pPlayerManager found [0x" << (int *)(dwPlayerManager - dwBase) << "]" << endl;
	}
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)dwAddCollectible, (PBYTE)original_addCollectible);
	DetourRemove((PBYTE)dwGameUpdate, (PBYTE)original_gameUpdate);
}
