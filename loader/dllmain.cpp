#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <curl.h>
#include <windows.h>
#include <tlhelp32.h>
#include "detours.h"

bool gbAttached = false;
HANDLE ghThread;

char* gIsaacUrl = "http://www.isaactracker.com";
char* gTrackerID = "64a3c29a-c71b-4b35-b08c-38d3f5a70586";

unsigned long GetModuleSize(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseSize;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}
unsigned long GetModuleBase(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseAddr;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}
unsigned long GetProcessId(wchar_t* pszProcessName)
{
	unsigned long dwResult = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
		if (Process32First(hSnapshot, &processEntry))
		{
			do
			{
				if (wcscmp(processEntry.szExeFile, pszProcessName) == 0)
				{
					dwResult = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &processEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}
DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (DWORD)(dwAddress + i);
	return 0;
}

bool(__cdecl* original_checkForGoldenKey)();
bool checkForGoldenKey()
{
	printf("checkForGoldenKey() Called\n");
	return true;
}

#pragma pack(1)
struct Entity
{
/*0x000*/	char _unk0000[0xC];
	int _id;
	unsigned int _variant;
	int _subtype;
	char _unk0018[0x10];
	void* Paralysis;
	char _unk0021[0x48];
	int _tearType;
	char _unk0020[0x23C];
	float _scaleX;
	float _scaleY;
	char _unk000D[0x88];
	float dmg;  // no
	float dmg2; // no
	char _unk000C[0xC8];
	float x;
	float y;
	char _unk0[0x350];
};

#pragma pack(1)
struct Player : Entity
{
	char _unk76C[0x3E4];
	int _maxHearts;
	int _hearts;
	int _eternalHearts;
	int _soulHearts;
	int _blackHeartMask;
	int _jarHearts;
	int _numKeys;
	int _hasGoldenKey;
	int _numBombs;
	int _numCoins;
	char _unk900[0x6C];
	float _shotspeed;
	char _unkA00[0x08];
	float _damage;
	float _range;
	char _unkB00[0xBC];
	float _speed;
	float _luck;
	char _unkC00[0xB4];
	BOOL _items[0x15A];
	char _unkB78[0x1D88];
};

DWORD WINAPI updateServer(void* pThreadArgument)
{
	try
	{
		Player* pPlayer = ((Player *)pThreadArgument);
		/* craft our json object to send to the server */
		char buffer1[1024] = { 0 };
		strcat_s(buffer1, 512, "[");
		char itemidbuffer[0x8];
		for (int i = 0; i < 0x15A; i++)
		{
			ZeroMemory(itemidbuffer, 0x8);
			if (pPlayer->_items[i])
			{
				sprintf_s(itemidbuffer, 0x8, "%d,", i + 1);
				strcat_s(buffer1, 1024, itemidbuffer);
			}
		}
		/* replace our trailing comma with a closing bracket */
		buffer1[strlen(buffer1) - 1] = ']';
		char buffer2[2048] = { 0 };
		sprintf_s(buffer2, 2048, "{\"coins\": %d, \"bombs\": %d, \"keys\": %d, \"items\": %s}", pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, buffer1);

		CURL *curl;
		char finalUrl[256] = { 0 };
		sprintf_s(finalUrl, 256, "%s/api/%s/pickup/", gIsaacUrl, gTrackerID);
		curl = curl_easy_init();
		if (curl) {
			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer2);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(curl, CURLOPT_URL, finalUrl);
			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
	}
	catch (int){}
	return 0;
}

Player* gpPlayer = NULL;
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
	printf("pPlayer [0x%X]\n", pPlayer);
	//store our player pointer if we dont have it
	if (gpPlayer != pPlayer)
		gpPlayer = pPlayer;
	int ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	/* update server! */
	CreateThread(NULL, 0, updateServer, pPlayer, 0L, NULL);
	return ret;
}

DWORD WINAPI DllThread(void* pThreadArgument)
{
	/* attach our console to our injector's console */
	AttachConsole(GetProcessId(L"injector.exe"));
	freopen("CONOUT$", "w", stdout);
	printf("DLL Attached!\n");
	/* init curl */
	curl_global_init(CURL_GLOBAL_ALL);
	/* scan for functions */
	unsigned long dwPid = GetProcessId(L"isaac-ng.exe");
	unsigned long dwSize = 0;
	unsigned long dwBase = GetModuleBase(dwPid, L"isaac-ng.exe", &dwSize);
	printf("dwBase [0x%X]\n", dwBase);
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	DWORD dwAddCollectible = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");

	if (dwAddCollectible)
	{
		printf("dwItemPickup found [0x%X]\n", dwAddCollectible - dwBase);
		original_addCollectible = (int(__fastcall *)(Player*, int, int, int, int))DetourFunction((PBYTE)dwAddCollectible, (PBYTE)addCollectible);
	}

	/* wait to be detached */
	while (gbAttached){ Sleep(100); }
	printf("DLL Detached!\n");
	/* un-detour functions */
	DetourRemove((PBYTE)original_addCollectible, (PBYTE)addCollectible);
	return 0;
}


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		/* use a global attached variable and a thread to make sure we don't crash if isaac is unloaded from under us */
		if (!gbAttached)
		{
			gbAttached = true;
			//everything should be done in a thread instead of as part of the process attach call 
			ghThread = CreateThread(NULL, 0, DllThread, NULL, 0L, NULL);
		}
		break;
	case DLL_PROCESS_DETACH:
		gbAttached = false;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}