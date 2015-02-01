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

struct Entity
{
/*0x0000*/	char _unk0000[0xC];
/*0x000C*/	int _id;
/*0x0010*/	unsigned int _variant;
/*0x0014*/	int _subtype;
/*0x0018*/	char _unk0018[0x10];
/*0x0028*/	void* Paralysis;
/*0x002C*/	char _unk002C[0x48];
/*0x0074*/	int _tearType;
/*0x0078*/	char _unk0078[0x23C];
/*0x02B4*/	float _scaleX;
/*0x02B8*/	float _scaleY;
/*0x02BC*/	char _unk02BC[0x88];
/*0x0344*/	float dmg;  // no
/*0x0348*/	float dmg2; // no
/*0x034C*/	char _unk034C[0xC8];
/*0x0414*/	float x;
/*0x0418*/	float y;
/*0x041C*/	char _unk041C[0x350];
}; /*0x076C*/

struct Player : Entity
{
/*0x076C*/	char _unk76C[0x3E4];
/*0x0B50*/	int _maxHearts;
/*0x0B54*/	int _hearts;
/*0x0B58*/	int _eternalHearts;
/*0x0B5C*/	int _soulHearts;
/*0x0B60*/	int _blackHeartMask;
/*0x0B64*/	int _jarHearts;
/*0x0B68*/	int _numKeys;
/*0x0B6C*/	int _hasGoldenKey;
/*0x0B70*/	int _numBombs;
/*0x0B74*/	int _numCoins;
/*0x0B78*/	char _unk0B78[0x24];
/*0x0B9C*/	char* _characterName;
/*0x0BA0*/	char _unk0BA0[0x3C];
/*0x0BDC*/	float _shotspeed;
/*0x0BE0*/	char _unk0BEO[0x08];
/*0x0BE8*/	float _damage;
/*0x0BEC*/	float _range;
/*0x0BF0*/	char _unk0BF0[0xBC];
/*0x0CAC*/	float _speed;
/*0x0CB0*/	float _luck;
/*0x0CB4*/	char _unk0CC4[0x18];
/*0x0CCC*/	int _charges;
/*0x0CD0*/	char _unk0CD0[0x94];
/*0x0D64*/	int _trinketID;
/*0x0D68*/	char _unk0D68[0x8];
/*0x0D74*/	BOOL _items[0x15A];
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