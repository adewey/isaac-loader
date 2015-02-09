#include "..\loader\plugin.h"
#include "curl.h"

char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "http://www.isaactracker.com";

bool bAttached = false;
bool bUpdateRequired = false;

DWORD WINAPI updateServer(void *pThreadArgument)
{
	while (bAttached)
	{
		if (bUpdateRequired && GetPlayer())
		{
			Player *pPlayer = GetPlayer();
			/* craft our json object to send to the server */
			/* craft our item array */
			char itembuffer[1024] = { 0 };
			strcat_s(itembuffer, 1024 - 1, "[");
			char itemidbuffer[0x32];
			for (int i = 0; i < 0x15A; i++)
			{
				ZeroMemory(itemidbuffer, 0x32);
				if (pPlayer->_items[i])
				{
					sprintf_s(itemidbuffer, 0x32, "%d,", i + 1);
					strcat_s(itembuffer, 1024 - 1, itemidbuffer);
				}
			}
			/* replace our trailing comma with a closing bracket */
			itembuffer[strlen(itembuffer) - 1] = ']';

			/* craft our trinket array */
			char trinketbuffer[1024] = { 0 };
			strcat_s(trinketbuffer, 1024, "[");
			char trinketidbuffer[0x32];
			ZeroMemory(trinketidbuffer, 0x32 - 1);
			sprintf_s(trinketidbuffer, 0x32 - 1, "%d,", pPlayer->_trinket1ID);
			strcat_s(trinketbuffer, 1024 - 1, trinketidbuffer);
			ZeroMemory(trinketidbuffer, 0x32 - 1);
			sprintf_s(trinketidbuffer, 0x32 - 1, "%d,", pPlayer->_trinket2ID);
			strcat_s(trinketbuffer, 1024 - 1, trinketidbuffer);
			/* replace our trailing comma with a closing bracket */
			trinketbuffer[strlen(trinketbuffer) - 1] = ']';

			/* craft our pocket array */
			char pocketbuffer[1024] = { 0 };
			strcat_s(pocketbuffer, 1024 - 1, "[");
			char pocketidbuffer[0x32];
			ZeroMemory(pocketidbuffer, 0x32 - 1);
			sprintf_s(pocketidbuffer, 0x32 - 1, "{\"id\": %d, \"is_card\": %d},", pPlayer->_pocket1ID, pPlayer->_pocket1isCard);
			strcat_s(pocketbuffer, 1024 - 1, pocketidbuffer);
			ZeroMemory(pocketidbuffer, 0x32 - 1);
			sprintf_s(pocketidbuffer, 0x32 - 1, "{\"id\": %d, \"is_card\": %d},", pPlayer->_pocket2ID, pPlayer->_pocket2isCard);
			strcat_s(pocketbuffer, 1024 - 1, pocketidbuffer);
			/* replace our trailing comma with a closing bracket */
			pocketbuffer[strlen(pocketbuffer) - 1] = ']';

			char buffer2[2048] = { 0 };
			sprintf_s(buffer2, 2048 - 1, "{\"character\": \"%s\", \"characterid\": %d, \"coins\": %d, \"bombs\": %d, \"keys\": %d, \"items\": %s, \"trinkets\": %s, \"pockets\": %s}",pPlayer->_characterName, pPlayer->_charID, pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, itembuffer, trinketbuffer, pocketbuffer);
			cout << buffer2 << endl;
			cout << pPlayer->_charID << endl;
			CURL *curl;
			char finalUrl[256] = { 0 };
			sprintf_s(finalUrl, 256 - 1, "%s/api/%s/pickup/", gIsaacUrl, gTrackerID);
			curl = curl_easy_init();
			if (curl)
			{
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
			bUpdateRequired = 0;
		}
		Sleep(1000);
	}
	return 0;
}

void setKey(int argc, char *argv[])
{
	strncpy_s(gTrackerID, argv[0], MAX_STRING);
	IniWriteString("tracker", "key", gTrackerID);
	cout << "Tracker ID set to: " << gTrackerID << endl;
}

void getKey(int argc, char *argv[])
{
	cout << "Your current tracker ID: " << gTrackerID << endl;
}

// called when the plugin initializes
PAPI VOID InitPlugin()
{
	bAttached = true;
	//Add commands, detours, etc
	AddCommand("setkey", setKey);
	AddCommand("getkey", getKey);

	IniReadString("tracker", "key", gTrackerID);
	CreateThread(NULL, 0, updateServer, NULL, 0L, NULL);
}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	bAttached = false;
	//remove commands, detours, etc
	RemoveCommand("setkey");
	RemoveCommand("getkey");
}

PAPI VOID OnAddCollectible(Player *pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	//do stuff with the collectible's information

	//at this time in the tracker we only care about sending player data to the server
	bUpdateRequired = true;
}

DWORD dwFrameCount = 0;
PAPI VOID OnGameUpdate()
{
	if (dwFrameCount > 60 * 60)
	{
		bUpdateRequired = true;
		dwFrameCount = 0;
	}
	dwFrameCount++;
}