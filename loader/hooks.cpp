#include "hooks.h"
#include "utilities.h"

//TODO(dither): move this updateserver into its own plugin
char* gIsaacUrl = "http://www.isaactracker.com";
char* gTrackerID = "64a3c29a-c71b-4b35-b08c-38d3f5a70586";

DWORD WINAPI updateServer(void* pThreadArgument)
{
	try
	{
		Player* pPlayer = ((Player *)pThreadArgument);
		/* craft our json object to send to the server */

		/* craft our item array */
		char itembuffer[1024] = { 0 };
		strcat_s(itembuffer, 512, "[");
		char itemidbuffer[0x8];
		for (int i = 0; i < 0x15A; i++)
		{
			ZeroMemory(itemidbuffer, 0x8);
			if (pPlayer->_items[i])
			{
				sprintf_s(itemidbuffer, 0x8, "%d,", i + 1);
				strcat_s(itembuffer, 1024, itemidbuffer);
			}
		}
		/* replace our trailing comma with a closing bracket */
		itembuffer[strlen(itembuffer) - 1] = ']';

		/* craft our trinket array */
		char trinketbuffer[1024] = { 0 };
		strcat_s(trinketbuffer, 512, "[");
		char trinketidbuffer[0x8];
		ZeroMemory(trinketidbuffer, 0x8);
		sprintf_s(trinketidbuffer, 0x8, "%d,", pPlayer->_trinket1ID);
		strcat_s(trinketbuffer, 1024, trinketidbuffer);
		ZeroMemory(trinketidbuffer, 0x8);
		sprintf_s(trinketidbuffer, 0x8, "%d,", pPlayer->_trinket2ID);
		strcat_s(trinketbuffer, 1024, trinketidbuffer);
		/* replace our trailing comma with a closing bracket */
		trinketbuffer[strlen(trinketbuffer) - 1] = ']';

		/* craft our pocket array */
		char pocketbuffer[1024] = { 0 };
		strcat_s(pocketbuffer, 512, "[");
		char pocketidbuffer[0x32];
		ZeroMemory(pocketidbuffer, 0x32);
		sprintf_s(pocketidbuffer, 0x32, "{\"id\": %d, \"is_card\": %d},", pPlayer->_pocket1ID, pPlayer->_pocket1isCard);
		strcat_s(pocketbuffer, 1024, pocketidbuffer);
		ZeroMemory(pocketidbuffer, 0x32);
		sprintf_s(pocketidbuffer, 0x32, "{\"id\": %d, \"is_card\": %d},", pPlayer->_pocket2ID, pPlayer->_pocket2isCard);
		strcat_s(pocketbuffer, 1024, pocketidbuffer);
		/* replace our trailing comma with a closing bracket */
		pocketbuffer[strlen(pocketbuffer) - 1] = ']';

		char buffer2[2048] = { 0 };
		sprintf_s(buffer2, 2048, "{\"coins\": %d, \"bombs\": %d, \"keys\": %d, \"items\": %s, \"trinkets\": %s, \"pockets\": %s}", pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, itembuffer, trinketbuffer, pocketbuffer);

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
	int ret = original_addCollectible(pPlayer, relatedID, itemID, charges, arg5);
	/* update server! */
	CreateThread(NULL, 0, updateServer, pPlayer, 0L, NULL);
	return ret;
}

/* currently unused */
bool(__cdecl* original_checkForGoldenKey)();
bool checkForGoldenKey()
{
	printf("checkForGoldenKey() Called\n");
	return true;
}





/* add other global hooks here, and expose them for our events to catch */
DWORD dwAddCollectible = 0;

void InitHooks()
{
	/* todo move this somewhere else? init curl */
	curl_global_init(CURL_GLOBAL_ALL);

	/* scan for functions */
	unsigned long dwPid = GetProcessId(L"isaac-ng.exe");
	unsigned long dwSize = 0;
	unsigned long dwBase = GetModuleBase(dwPid, L"isaac-ng.exe", &dwSize);
	printf("dwBase [0x%X]\n", dwBase);
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	dwAddCollectible = dwFindPattern(dwBase, dwSize,
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
}

void RemoveHooks()
{
	/* un-detour functions */
	DetourRemove((PBYTE)dwAddCollectible, (PBYTE)original_addCollectible);
}
