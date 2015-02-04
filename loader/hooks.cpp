#include "hooks.h"
#include "utilities.h"

/*
addCollectible is called when you pick up a pedestal, shop, market, or devil/angel item
it includes active(spacebar), passive, and familiar(followers)
:params pPlayer: a pointer to our player entity
:params relatedID: relatedID != itemID if you start with the item, otherwise relatedID and itemID are the same for pickups
:params charges: number of charges on the item currently (0-6 for standard items, 0x100+ for items that recharge in room)
:params arg5: currently unknown what this is for (maybe a boolean value? i have only seen 1 or 0
*/
int(__fastcall *original_addCollectible)(Player* pPlayer, int relatedID, int itemID, int charges, int arg5);
int __fastcall addCollectible(Player* pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	if (gpPlayer != pPlayer) gpPlayer = pPlayer;
	cout << "pPlayer [0x" << pPlayer << "]" << endl;
	int ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	/* update server! */
	return ret;
}

#define printarg(i, arg) cout << "Arg" << i << ":\t" << (void *)arg << "\t" << (float)arg << endl;

/*Clueless, mindlessly copied code.*/
int(__stdcall *original_spawnEntity)(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9);
int __stdcall spawnEntity(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9) {
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

/* add other global hooks here, and expose them for our events to catch */
DWORD dwAddCollectible = 0;
DWORD dwSpawnEntity = 0;
DWORD dwPlayerManager = 0;
void InitHooks()
{
	/* todo move this somewhere else? init curl */
	curl_global_init(CURL_GLOBAL_ALL);

	/* scan for functions */
	unsigned long dwPid = GetProcessId(L"isaac-ng.exe");
	unsigned long dwSize = 0;
	unsigned long dwBase = GetModuleBase(dwPid, L"isaac-ng.exe", &dwSize);
	cout << "dwBase [0x" << (void *)dwBase << "]" << endl;
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	dwAddCollectible = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");

	if (dwAddCollectible){
		cout << "dwItemPickup found [0x" << (void *)(dwAddCollectible - dwBase) << "]" << endl;
		original_addCollectible = (int(__fastcall *)(Player*, int, int, int, int))DetourFunction((PBYTE)dwAddCollectible, (PBYTE)addCollectible);
	}

	dwSpawnEntity = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8b\xec\x51\x8b\x4d\x08\x8b\x89\xa4\xe2\x12\x00\x56\x57\x8b\xf8\x8b\x45\x0c"
		"\xe8\x00\x00\x00\x00\x8b\xf0\xff\x15\x00\x00\x00\x00\x8b\x4d\x18\x8b\x16\x8b\x52"
		"\x04\x89\x45\xfc\x8b\x45\x1c\x50\x8b\x45\x10\x51\x8b\x4d\x0c\x50\x51\x8b\xce\xff"
		"\xd2",
		"xxxxxxxxxxxxxxxxxxxxx????xxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	if (dwSpawnEntity){
		cout << "dwSpawnEntity found [0x" << (void *)(dwSpawnEntity - dwBase) << "]" << endl;
		//original_spawnEntity = (int(__stdcall *)(int, int, int, int, int, int, int, int, int))DetourFunction((PBYTE)dwSpawnEntity, (PBYTE)spawnEntity);
	}

	dwPlayerManager = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x8b\x0d\x00\x00\x00\x00\x8b\x81\x9c\x94\x00\x00\x2b\x81\x98\x94\x00\x00\xc1\xf8\x02\xc3",
		"xx????xxxxxxxxxxxxxxxx");

	if (dwPlayerManager){
		cout << "dwPlayerManager found [0x" << (void *)(dwPlayerManager) << "]" << endl;
	}
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)dwAddCollectible, (PBYTE)original_addCollectible);
}
