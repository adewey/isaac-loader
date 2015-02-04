#include "../loader/plugin.h"

char *gTrackerID = "64a3c29a-c71b-4b35-b08c-38d3f5a70586";
char* gIsaacUrl = "http://www.isaactracker.com";

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

void setKey(Player *pPlayer, int argc, char *argv[])
{
	gTrackerID = argv[0];
	cout << "Tracker ID set to: " << gTrackerID << endl;
}

void getKey(Player *pPlayer, int argc, char *argv[])
{
	cout << "Your current tracker ID: " << gTrackerID << endl;
}

// called when the plugin initializes
PAPI VOID InitPlugin()
{
	//Add commands, detours, etc
	AddCommand("setkey", setKey);
	AddCommand("getkey", getKey);
}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	//remove commands, detours, etc
	RemoveCommand("setkey");
	RemoveCommand("getkey");
}

PAPI VOID OnAddCollectible(Player* pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	//do stuff with the collectible's information

	//at this time in the tracker we only care about sending player data to the server
	CreateThread(NULL, 0, updateServer, pPlayer, 0L, NULL);
}