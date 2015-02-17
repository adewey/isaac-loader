#include "..\loader\plugin.h"
#include "curl.h"
#include "easywsclient.hpp"
#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <vector>
#include <stdio.h>
#include <string>
char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "http://www.isaactracker.com";

bool bAttached = false;
bool bUpdateRequired = false;
bool bShouldUpdate = false;

vector<string> SendToServer;

using easywsclient::WebSocket;
static WebSocket::pointer websocket = NULL;

void handle_message(const std::string & message)
{
	cout << "Message Received from Server: " << message;
}
//DWORD WINAPI startSocket(void *pThreadArgument){
//#ifdef _WIN32
//	INT rc;
//	WSADATA wsaData;
//
//	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (rc) {
//		printf("WSAStartup Failed.\n");
//	}
//#endif
//	websocket = from_url("ws://isaactracker.com:8126/", false, "Client");
//	if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED){
//		cout << "Could Not Connect to Web Socket." << endl;
//	}
//	else{
//		cout << "Web socket Connected" << endl;
//		while (websocket->getReadyState() != WebSocket::CLOSED) {
//			websocket->poll();
//			websocket->dispatch(handle_message);
//			if (!bAttached) break;
//		}
//		cout << "Websocket disconnected" << endl;
//	}
//	return 0;
//}

DWORD WINAPI SocketHandler(void *pThreadArgument){
#ifdef _WIN32
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
	}
#endif
	websocket = from_url("ws://isaactracker.com:8126/", false, "Client");
	if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED){
		cout << "Could Not Connect to Web Socket." << endl;
	}
	else{
		cout << "Web socket Connected" << endl;
		while (websocket->getReadyState() != WebSocket::CLOSED) {
			websocket->poll();
			websocket->dispatch(handle_message);
			if (!SendToServer.empty()){
				websocket->send(SendToServer.back());
				SendToServer.pop_back();
			}
			if (!bAttached) break;
		}
		cout << "Websocket disconnected" << endl;
	}
	return 0;
}

void SendMessage(string message){
	cout << "'" << message << "'" << endl;
	SendToServer.push_back(message);
}

void terminateSocket(){
	websocket->close();
	delete websocket;
	#ifdef _WIN32
		WSACleanup();
	#endif
}
DWORD WINAPI updateServer(void *pThreadArgument)
{
	while (bAttached)
	{
		if (bUpdateRequired && GetPlayerEntity())
		{
			Player *pPlayer = GetPlayerEntity();
			PPLAYERMANAGER pPlayerManager = GetPlayerManager();
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
			SendMessage((string)buffer2);
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
	CreateThread(NULL, 0, SocketHandler, NULL, 0L, NULL);
}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	terminateSocket();
	bAttached = false;
	//remove commands, detours, etc
	RemoveCommand("setkey");
	RemoveCommand("getkey");
}

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

DWORD dwFrameCount = 60;
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