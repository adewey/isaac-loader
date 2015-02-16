#include "hooks.h"
#include "utilities.h"
#include "plugins.h"
#include "statics.h"

DWORD gdwBaseAddress = (DWORD)GetModuleHandle(NULL);
DWORD gdwBaseSize = (DWORD)dwGetModuleSize("isaac-ng.exe");

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
	PreAddCollectible(pPlayer, &relatedID, &itemID, &charges, &arg5);
	int	ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	OnAddCollectible(pPlayer, relatedID, itemID, charges, arg5);
	PostAddCollectible(ret);
	return ret;
}

DWORD dwChangeKeys = 0;
int(__fastcall *original_changeKeys)(Player *pPlayer, int _EDX, int nKeys);
int __fastcall changeKeys(Player *pPlayer, int _EDX, int nKeys)
{
	PreChangeKeys(pPlayer, &nKeys);;
	int	ret = original_changeKeys(pPlayer, _EDX, nKeys);
	OnChangeKeys(pPlayer, nKeys);
	PostChangeKeys(ret);
	return ret;
}

DWORD dwChangeBombs = 0;
int(__fastcall *original_changeBombs)(Player *pPlayer, int _EDX, int nBombs);
int __fastcall changeBombs(Player *pPlayer, int _EDX, int nBombs)
{
	PreChangeBombs(pPlayer, &nBombs);
	int	ret = original_changeBombs(pPlayer, _EDX, nBombs);
	OnChangeBombs(pPlayer, nBombs);
	PostChangeBombs(ret);
	return ret;
}

DWORD dwChangeCoins = 0;
DWORD original_changeCoins = 0;
void* coins_retAddr = NULL;
__declspec(naked) char changeCoins()
{
	_asm
	{
		push ebp
			mov ebp, esp
				push dword ptr[ebp + 0x08]
				push eax
				call PreChangeCoins
				pop eax
				add esp, 4
				push dword ptr[ebp + 0x08]
				push eax
				call OnChangeCoins
				pop eax
				add esp, 4
			pop ebp
			pop coins_retAddr
			call original_changeCoins
			push eax
				push eax
				call PostChangeCoins
				add esp, 4
			pop eax
			push coins_retAddr
			ret
	}
}



//HEY DOOFUS, ADD THAT FUNCTION
void AddCollectible(Player *player, int itemID){
	addCollectible(player, itemID, itemID, 6, 1);
}

/* SpawnEntity functions */
/* TODO(Aaron): figure out a better way to handle this so we can detour properly */
DWORD gdwSpawnEntity = 0;
void* SpawnEntityEvent_Original;
void __cdecl SpawnEntityEvent_Payload(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, Entity *parent, int subtype, unsigned int seed)
{
	if (entityID != 1000){
		PreSpawnEntity(velocity, position, &playerManager, &entityID, &variant, parent, &subtype, &seed);
		OnSpawnEntity(velocity, position, playerManager, entityID, variant, parent, subtype, seed);
	}
	//do things with spawn entity
	//OnSpawnEntity();
}
__declspec(naked) char SpawnEntityEvent_Hook()
{
	_asm
	{
		push ebp
			mov ebp, esp
			push eax
			push ebx
			push dword ptr[ebp + 0x1C]
			push dword ptr[ebp + 0x18]
			push dword ptr[ebp + 0x14]
			push dword ptr[ebp + 0x10]
			push dword ptr[ebp + 0x0C]
			push dword ptr[ebp + 0x08]
			push ebx
			push eax
			call SpawnEntityEvent_Payload
			add esp, 32
			pop ebx
			pop eax
			pop ebp
			jmp SpawnEntityEvent_Original
	}
}

/* isaac random function used in many things */
DWORD dwIsaacRandom = 0;
int(__cdecl* original_IsaacRandom)();
int IsaacRandom()
{
	return original_IsaacRandom();
}

/* GameUpdate Function */
DWORD gdwGameUpdate = 0;
void(__cdecl *original_gameUpdate)();
void __cdecl GameUpdate()
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
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)gdwGameUpdate, (PBYTE)GameUpdate);
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

	dwIsaacRandom = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\xa1\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x0f\x8c\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x75"
		"\x00\xb8\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x33\xc9\xeb\x00\x8d\xa4\x24\x00\x00\x00\x00"
		"\x8d\x64\x24\x00\x8b\x04\x8d\x00\x00\x00\x00\x33\x04\x8d\x00\x00\x00\x00\x41\x25\x00\x00"
		"\x00\x00\x33\x04\x8d\x00\x00\x00\x00\x8b\xd0\xd1\xe8\x83\xe2\x01\x33\x04\x95\x00\x00\x00"
		"\x00\x33\x04\x8d\x00\x00\x00\x00\x89\x04\x8d\x00\x00\x00\x00\x81\xf9\x00\x00\x00\x00\x7c"
		"\x00\x81\xf9\x00\x00\x00\x00\x7d\x00\x8d\x0c\x8d\x00\x00\x00\x00\x8b\xff",
		"x????x????xx????x????x?x????x????xxx?xxxxxxxxxxxxxx????xxx????xx????xxx????xxxxxxxxxx???"
		"?xxx????xxx????xx????x?xx????x?xxx????xx");

	if (dwIsaacRandom)
	{
		original_IsaacRandom = (int(__cdecl *)())DetourFunction((PBYTE)dwIsaacRandom, (PBYTE)IsaacRandom);
	}

	gdwSpawnEntity = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x51\x8b\x4d\x08\x8b\x89\x04\xd8\x10\x00\x56\x57\x8b\xf8\x8b\x45\x0c",
		"xxxxxxxxx????xxxxxxx");

	if (gdwSpawnEntity)
	{
		SpawnEntityEvent_Original = (void *)DetourFunction((PBYTE)gdwSpawnEntity, (PBYTE)SpawnEntityEvent_Hook);
	}

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

	dwChangeCoins = gdwBaseAddress + 0xCEAB0;

	if (dwChangeCoins)
	{
		original_changeCoins = (DWORD)DetourFunction((PBYTE)dwChangeCoins, (PBYTE)changeCoins);
	}

	/* todo: replace this with a sig */
	gdwGetPlayerEntity = gdwBaseAddress + 0xFA50;
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)gdwAddCollectible, (PBYTE)original_addCollectible);
	DetourRemove((PBYTE)gdwGameUpdate, (PBYTE)original_gameUpdate);
	DetourRemove((PBYTE)dwIsaacRandom, (PBYTE)original_IsaacRandom);
}
