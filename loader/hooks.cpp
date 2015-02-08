#include "hooks.h"
#include "utilities.h"
#include "plugins.h"
#include "statics.h"

DWORD gdwBaseAddress = (DWORD)GetModuleHandle(NULL);
DWORD gdwBaseSize = (DWORD)dwGetModuleSize("isaac-ng.exe");

#define printarg(i, arg) cout << "Arg" << i << ":\t" << (void *)arg << "\t" << (float)arg << endl;

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
	OnAddCollectible(pPlayer, relatedID, itemID, charges, arg5);

	return ret;
}


/* SpawnEntity functions */
/* TODO(Aaron): figure out a better way to handle this so we can detour properly */
DWORD dwSpawnEntity = 0;
void* SpawnEntityEvent_Original;
void __cdecl SpawnEntityEvent_Payload(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, ENTITY *parent, int subtype, unsigned int seed)
{
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

/* currently unused */
bool(__cdecl* original_checkForGoldenKey)();
bool checkForGoldenKey()
{
	cout << "checkForGoldenKey() Called" << endl;
	return original_checkForGoldenKey();
}

/* isaac random function used in many things */
DWORD dwIsaacRandom = 0;
int(__cdecl* original_IsaacRandom)();
int IsaacRandom()
{
	return original_IsaacRandom();
}

/* GameUpdate Function */
DWORD dwGameUpdate = 0;
void(__cdecl *original_gameUpdate)();
void __cdecl GameUpdate()
{
	original_gameUpdate();
	OnGameUpdate();
}

/* add other global hooks here, and expose them for our events to catch */
DWORD gdwPlayerManager = 0;

void InitHooks()
{
	cout << "gdwBaseAddress [0x" << (void *)gdwBaseAddress << "]" << endl;
	/* scan for functions */
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	dwGameUpdate = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x83\xec\x0c\x53\x8b\x1d\x00"
		"\x00\x00\x00\x56\x57\x8d\x83\xa4\xa5\x02\x00\x50"
		"\x89\x5d\xf8",
		"xxxxxxxxx????xxxx????xxxx");

	if (dwGameUpdate)
	{
		cout << "Found dwGameUpdate: [0x" << (void *)(dwGameUpdate - gdwBaseAddress) << "]" << endl;
		original_gameUpdate = (void(__cdecl *)())DetourFunction((PBYTE)dwGameUpdate, (PBYTE)GameUpdate);
	}

	dwAddCollectible = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");

	if (dwAddCollectible)
	{
		cout << "dwItemPickup found [0x" << (void *)(dwAddCollectible - gdwBaseAddress) << "]" << endl;
		original_addCollectible = (int(__fastcall *)(PPLAYER, int, int, int, int))DetourFunction((PBYTE)dwAddCollectible, (PBYTE)addCollectible);
	}

	gdwPlayerManager = *(DWORD*)(dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x8b\x0d\x00\x00\x00\x00\x8b\x81\x30\x8a\x00\x00\x2b\x81\x2c"
		"\x8a\x00\x00\xc1\xf8\x02\xc3",
		"xx????xx????xx????xxxx") + 2);

	if (gdwPlayerManager)
	{
		cout << "gdwPlayerManager found [0x" << (void *)(gdwPlayerManager - gdwBaseAddress) << "] (holds pointer for PlayerManager)" << endl;
	}

	dwIsaacRandom = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)	"\xa1\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x0f\x8c\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x75"
	"\x00\xb8\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x33\xc9\xeb\x00\x8d\xa4\x24\x00\x00\x00\x00"
	"\x8d\x64\x24\x00\x8b\x04\x8d\x00\x00\x00\x00\x33\x04\x8d\x00\x00\x00\x00\x41\x25\x00\x00"
	"\x00\x00\x33\x04\x8d\x00\x00\x00\x00\x8b\xd0\xd1\xe8\x83\xe2\x01\x33\x04\x95\x00\x00\x00"
	"\x00\x33\x04\x8d\x00\x00\x00\x00\x89\x04\x8d\x00\x00\x00\x00\x81\xf9\x00\x00\x00\x00\x7c"
	"\x00\x81\xf9\x00\x00\x00\x00\x7d\x00\x8d\x0c\x8d\x00\x00\x00\x00\x8b\xff",
	"x????x????xx????x????x?x????x????xxx?xxxxxxxxxxxxxx????xxx????xx????xxx????xxxxxxxxxx???"
	"?xxx????xxx????xx????x?xx????x?xxx????xx");

	if (dwIsaacRandom)
	{
		cout << "Found dwIsaacRandom: [0x" << (void *)(dwIsaacRandom - gdwBaseAddress) << "]" << endl;
		original_IsaacRandom = (int(__cdecl *)())DetourFunction((PBYTE)dwIsaacRandom, (PBYTE)IsaacRandom);
	}

	dwSpawnEntity = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x55\x8b\xec\x51\x8b\x4d\x08\x8b\x89\xfc\xd7\x10\x00\x56"
		"\x57\x8b\xf8\x8b\x45\x0c\xe8\x00\x00\x00\x00\x8b\xf0",
		"xxxxxxxxxxxxxxxxxxxxx????xx");

	if (dwSpawnEntity)
	{
		cout << "Found dwSpawnEntity: [0x" << (void *)(dwSpawnEntity - gdwBaseAddress) << "]" << endl;
		SpawnEntityEvent_Original = (void *)DetourFunction((PBYTE)dwSpawnEntity, (PBYTE)SpawnEntityEvent_Hook);
	}
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)dwAddCollectible, (PBYTE)original_addCollectible);
	DetourRemove((PBYTE)dwGameUpdate, (PBYTE)original_gameUpdate);
	DetourRemove((PBYTE)dwIsaacRandom, (PBYTE)original_IsaacRandom);
}
