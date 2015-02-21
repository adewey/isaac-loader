#include "..\loader\plugin.h"
#include "easywsclient.hpp"
#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "http://www.isaactracker.com";

bool bAttached = false;
bool bUpdateRequired = false;

using easywsclient::WebSocket;
static WebSocket::pointer websocket = NULL;
vector<string> SendToServer;

void handle_message(const std::string & message)
{
	cout << "Message Received from Server: " << message;
}

DWORD WINAPI socketHandler(void *pThreadArgument){
	while (bAttached){
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
		}
#endif
		websocket = from_url("ws://www.isaactracker.com:7002/", false, "Client");;
		if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED){
		}
		else{
			while (websocket->getReadyState() != WebSocket::CLOSED) {
				websocket->poll();
				websocket->dispatch(handle_message);
				if (!SendToServer.empty()){
					websocket->send(SendToServer.back());
					SendToServer.pop_back();
				}
				if (!bAttached) break;
			}
		websocket->close();
		delete websocket;
		}
#ifdef _WIN32
		WSACleanup();
#endif
	}
	return 0;
}

void SendMessage(string message){
	SendToServer.push_back(message);
}


DWORD WINAPI updateServer(void *pThreadArgument)
{
	while (bAttached && gbAttached)
	{
		if (bUpdateRequired && GetPlayerEntity())
		{
			Player *pPlayer = GetPlayerEntity();
			PlayerManager *pPlayerManager = GetPlayerManager();
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
			sprintf_s(buffer2, 2048 - 1, "{\"stream_key\": \"%s\",\"character\": \"%s\", \"characterid\": \"%d\", \"seed\": \"%s\", \"floor\": \"%d\", \"altfloor\": \"%d\", \"curses\": \"%d\", \"guppy\": \"%d\", \"lof\": \"%d\", \"charges\": \"%d\", \"speed\": \"%0.2f\", \"shotspeed\": \"%0.2f\", \"tearrate\": %d, \"damage\": \"%0.2f\", \"luck\": \"%0.2f\", \"range\": \"%0.2f\", \"coins\": \"%d\", \"bombs\": \"%d\", \"keys\": \"%d\", \"items\": %s, \"trinkets\": %s, \"pockets\": %s}",
				gTrackerID, pPlayer->_characterName, pPlayer->_charID, pPlayerManager->_startSeed, pPlayerManager->_floorNo, pPlayerManager->_alternateFloor, pPlayerManager->i_curses, pPlayer->_nGuppyItems, pPlayer->_nFlyItems, pPlayer->_charges, pPlayer->_speed, pPlayer->_shotspeed, pPlayer->_tearrate, pPlayer->_damage, pPlayer->_luck, pPlayer->_range, pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, itembuffer, trinketbuffer, pocketbuffer);
			cout << buffer2 << endl;
			SendMessage((string)buffer2);
			bUpdateRequired = 0;
		}
		Sleep(1000);
	}
	return 0;
}

PAPI VOID InitPlugin()
{
	bAttached = true;

	IniReadString("tracker", "key", gTrackerID);
	CreateThread(NULL, 0, updateServer, NULL, 0L, NULL);
	CreateThread(NULL, 0, socketHandler, NULL, 0L, NULL);
}

PAPI VOID UnInitPlugin(VOID)
{
	bAttached = false;
}

bool bShouldUpdate = false;
PAPI VOID PostAddCollectible(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostChangeKeys(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostChangeBombs(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostChangeCoins(int ret)
{
	bShouldUpdate = true;
}

DWORD dwFrameCount = 30 * 60;
PAPI VOID OnGameUpdate()
{
	/* limit updates to once every 30 frames, then wait to update again until we need to */
	if (dwFrameCount > 60 * 30 && bShouldUpdate && !bUpdateRequired)
	{
		bShouldUpdate = false;
		bUpdateRequired = true;
		dwFrameCount = 0;
	}
	dwFrameCount++;
}
