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
#include <iostream>
#include <fstream>
char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "ws://ws.isaactracker.com";

bool bAttached = false;
bool bUpdateRequired = false;
ofstream trackerLogFile;
using easywsclient::WebSocket;
static WebSocket::pointer websocket = NULL;
vector<string> SendToServer;


void Log(string message){
	if (trackerLogFile.is_open()){
		trackerLogFile << "LOG: " << message << endl;
	}
}

void handle_message(const std::string & message)
{
	string messageLog = "Received Message from server: ";
	messageLog.append(message);
	Log(messageLog);
}

DWORD WINAPI socketHandler(void *pThreadArgument){
	while (bAttached){
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			Log("WSAStartup Failed.");
		}
#endif
		websocket = from_url(gIsaacUrl, false, "Client");;
		if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED){
			Log("Websocket Could not Connect.");
		}
		else{
			Log("Websocket Connected.");
			while (websocket->getReadyState() != WebSocket::CLOSED) {
				websocket->poll();
				websocket->dispatch(handle_message);
				if (!SendToServer.empty()){
					websocket->send(SendToServer.back());
					SendToServer.pop_back();
				}
				if (!bAttached){
					break;
				}
			}
			websocket->close();

			delete websocket;
		}
#ifdef _WIN32
		WSACleanup();
#endif
		Log("Web Socket Closed.");
	}
	Log("No Longer Attached to Isaac");
	return 0;
}

void SendMessage(string message){
	SendToServer.push_back(message);
}

PAPI VOID InitPlugin()
{
	bAttached = true;
	trackerLogFile.open("gemini/tracker_log.txt");
	Log("Tracker Started");
	IniReadString("tracker", "key", gTrackerID);
	string trackerLog = "Tracker ID: ";
	trackerLog.append(gTrackerID);
	Log(trackerLog);
	CreateThread(NULL, 0, socketHandler, NULL, 0L, NULL);
}

PAPI VOID UnInitPlugin(VOID)
{
	bAttached = false;
	trackerLogFile.close();
}



void updateServer()
{
	PlayerManager *pPlayerManager = GetPlayerManager();
	if (!pPlayerManager) return;
	Player *pPlayer = GetPlayerEntity();
	if (!pPlayer) return;
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
	if (strlen(itembuffer) == 1){
		itembuffer[0] = '[';
		itembuffer[1] = ']';
	}
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
	sprintf_s(buffer2, 2048 - 1, "{\"stream_key\": \"%s\",\"character\": \"%s\", \"characterid\": \"%d\", \"seed\": \"%s\", \"floor\": \"%d\", \"altfloor\": \"%d\", \"curses\": \"%d\", \"guppy\": \"%d\", \"lof\": \"%d\", \"charges\": \"%d\", \"speed\": \"%0.2f\", \"shotspeed\": \"%0.2f\", \"tearrate\": %d, \"damage\": \"%0.2f\", \"luck\": \"%0.2f\", \"range\": \"%0.2f\", \"coins\": \"%d\", \"bombs\": \"%d\", \"keys\": \"%d\", \"items\": %s, \"trinkets\": %s, \"pockets\": %s, \"hardmode\": %d}",
		gTrackerID, pPlayer->_characterName, pPlayer->_charID, pPlayerManager->_startSeed, pPlayerManager->_floorNo, pPlayerManager->_alternateFloor, pPlayerManager->i_curses, pPlayer->_nGuppyItems, pPlayer->_nFlyItems, pPlayer->_charges, pPlayer->_speed, pPlayer->_shotspeed, pPlayer->_tearrate, pPlayer->_damage, pPlayer->_luck, pPlayer->_range, pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, itembuffer, trinketbuffer, pocketbuffer, pPlayerManager->_hard_mode);
	SendMessage((string)buffer2);
}

bool bShouldUpdate = false;
PAPI VOID PostAddCollectible(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostAddKeys(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostAddBombs(int ret)
{
	bShouldUpdate = true;
}

PAPI VOID PostAddCoins(int ret)
{
	bShouldUpdate = true;
}

DWORD dwFrameCount = 0;
bool intro = false;
PAPI VOID OnGameUpdate()
{
	if (dwFrameCount >= (60 * 3) && intro)
	{
		intro = false;
		show_fortune_banner("", "isaactracker loaded!", "");
	}
	/* limit updates to once every 30 frames, then wait to update again until we need to */
	if (dwFrameCount >= (60 * 5) && bShouldUpdate)
	{
		bShouldUpdate = false;
		dwFrameCount = 0;
		updateServer();
	}
	dwFrameCount++;
}

PAPI VOID PostStartGame(int ret)
{
	dwFrameCount = 0;
	intro = true;
}

/* ret = boss id found in bossportraits.xml */
PAPI VOID PostTriggerBossDeath(int ret)
{

}
