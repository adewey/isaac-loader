#include "..\loader\plugin.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#include <map>
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
using namespace rapidjson;
bool bAttached = false;
bool bUpdateRequired = false;
ofstream trackerLogFile;
using easywsclient::WebSocket;
static WebSocket::pointer websocket = NULL;
vector<string> SendToServer;
vector<vector<string>> ShowWithBanner;

typedef map<string, string> MessageMap;
struct MessageHandler
	: public BaseReaderHandler<UTF8<>, MessageHandler> {
	MessageHandler() : messages_(), state_(kExpectObjectStart), name_() {}

	bool StartObject() {
		switch (state_) {
		case kExpectObjectStart:
			state_ = kExpectNameOrObjectEnd;
			//cout << "Expected start, starting" << endl;
			return true;
		default:
			//cout << "start, not expected. end" <<endl;
			return false;
		}
	}

	bool String(const char* str, SizeType length, bool) {
		switch (state_) {
		case kExpectNameOrObjectEnd:
			name_ = string(str, length);
			//cout << "String, Expected name or obj end" << endl;
			state_ = kExpectValue;
			return true;
		case kExpectValue:
			//cout << "String, Expected value" << endl;
			messages_.insert(MessageMap::value_type(name_, string(str, length)));
			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			//cout << "String, Not Expected" << endl;
			return false;
		}
	}
	bool Int(int i){
		switch (state_){
		case kExpectValue:
			//cout << "int, expected value " << endl;
			messages_.insert(MessageMap::value_type(name_,to_string(i)));

			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			//cout << "int, not expected" << endl;
			return false;
		}
	}

	bool EndObject(SizeType) { return state_ == kExpectNameOrObjectEnd; }

	bool Default() { return false; } // All other events are invalid.

	MessageMap messages_;
	enum State {
		kExpectObjectStart,
		kExpectNameOrObjectEnd,
		kExpectValue,
	}state_;
	std::string name_;
};

void Log(string message){
	if (trackerLogFile.is_open()){
		trackerLogFile << "LOG: " << message << endl;
	}
}

void ParseMessages(const char* json, MessageMap& messages) {
	Reader reader;
	MessageHandler handler;
	StringStream ss(json);
	if (reader.Parse(ss, handler))
		messages.swap(handler.messages_);   // Only change it if success.
	else {
		ParseErrorCode e = reader.GetParseErrorCode();
		size_t o = reader.GetErrorOffset();
		//cout << "Error: " << GetParseError_En(e) << endl;;
		//cout << " at offset " << o << " near '" << string(json).substr(o, 10) << "...'" << endl;
	}
}

void handle_message(const std::string & message)
{
	string messageLog = "Received Message from server: ";
	messageLog.append(message);
	Log(messageLog);

	MessageMap messages;
	ParseMessages(message.c_str(), messages);
	string action = "";
	for (MessageMap::const_iterator itr = messages.begin(); itr != messages.end(); ++itr){
		if (itr->first == "action"){
			cout << "Action: " << itr->second << endl;
			action = itr->second;
		}
	} //End initial loop to get action
	if (action == ""){
		Log("No action, doing nothing."); 
	}
	else if (action == "fortune"){
		vector<string> lines;
		for (MessageMap::const_iterator itr = messages.begin(); itr != messages.end(); ++itr){
			if (itr->first == "line1"){
				lines.push_back(itr->second);
			}
			else if (itr->first == "line2"){
				lines.push_back(itr->second);
			}
			else if (itr->first == "line3"){
				lines.push_back(itr->second);
			}
		} //End loop to get vals
		//can't actually handle 3 lines yet.... AARON!

		ShowWithBanner.push_back(lines);
	}
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
		websocket = from_url(gIsaacUrl, false, gTrackerID);;
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
		gTrackerID, pPlayer->_characterName, pPlayer->_charID, pPlayerManager->_startSeed, pPlayerManager->m_Stage, pPlayerManager->m_AltStage, pPlayerManager->i_curses, pPlayer->_nGuppyItems, pPlayer->_nFlyItems, pPlayer->_charges, pPlayer->_speed, pPlayer->_shotspeed, pPlayer->_tearrate, pPlayer->_damage, pPlayer->_luck, pPlayer->_range, pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, itembuffer, trinketbuffer, pocketbuffer, pPlayerManager->_hard_mode);
	SendMessage((string)buffer2);
}

bool bShouldUpdate = false;
PAPI VOID PostAddCollectible(int ret)
{
	StringBuffer s;

	Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.String("action");
		writer.String("updateCollectibles");
		writer.String("items");
		Player *pPlayer = GetPlayerEntity();
		if (!pPlayer){ return; }
		writer.StartArray();
		bool noItems = true;
			for (int i = 0; i < 0x15A; i++)
			{
				if (pPlayer->_items[i]){
					writer.Int(i+1);
					noItems = false;
				}
			}
			if (noItems){
				writer.Int(0);
			}
		writer.EndArray();
		writer.String("heldItemID");
		writer.Int(pPlayer->_helditemid);
		writer.String("numCharges");
		writer.Int(pPlayer->_charges);
		writer.String("luck");
		writer.Int(pPlayer->_luck);
		writer.String("damage");
		writer.Int(pPlayer->_damage);
		writer.String("range");
		writer.Int(pPlayer->_range);
		writer.String("shotspeed");
		writer.Int(pPlayer->_shotspeed);
		writer.String("tearrate");
		writer.Int(pPlayer->_tearrate);
		writer.String("speed");
		writer.Int(pPlayer->_speed);
	writer.EndObject();

	cout << s.GetString() << endl;
	SendMessage(s.GetString());
}

PAPI VOID PostAddKeys(int ret)
{
	StringBuffer s;

	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateKeys");
		writer.String("keys");
		writer.Int(ret);
	writer.EndObject();
	
	cout << s.GetString() << endl;
	SendMessage(s.GetString());
}

PAPI VOID PostAddBombs(int ret)
{
	StringBuffer s;

	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateBombs");
		writer.String("bombs");
		writer.Int(ret);
	writer.EndObject();

	cout << s.GetString() << endl;
	SendMessage(s.GetString());
}

PAPI VOID PostAddCoins(int ret)
{
	//bShouldUpdate = true;
}

DWORD dwFrameCount = 0;
int framesToNextBanner = 120;
bool intro = false;
PAPI VOID OnGameUpdate()
{
	if (dwFrameCount >= (60 * 3))
	{
	}
	/* limit updates to once every 30 frames, then wait to update again until we need to */
	if (dwFrameCount >= (60 * 5) && bShouldUpdate)
	{
		bShouldUpdate = false;
		dwFrameCount = 0;
		//updateServer();
	}
	if (!(ShowWithBanner.empty()) && (framesToNextBanner <= 0)){
		show_fortune_banner(ShowWithBanner.back().at(0).c_str(), ShowWithBanner.back().at(1).c_str(), ShowWithBanner.back().at(2).c_str());
		ShowWithBanner.pop_back();
		framesToNextBanner = 60;
	}
	dwFrameCount++;
	framesToNextBanner--;
}

PAPI VOID PostStartGame(int ret)
{
	dwFrameCount = 0;
}

/* ret = boss id found in bossportraits.xml */
PAPI VOID PostTriggerBossDeath(int ret)
{

}

PAPI VOID PostLevelInit(int ret)
{
}

//returning false here rerolls the item before the player has a chance to see it..
PAPI bool PostItemPool__GetCollectible(int id)
{
	return true;
}
