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
#include <deque>
#include <iostream>
#include <fstream>


double dTrackerVersion = 1.31;

char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "ws://ws.isaactracker.com";
using namespace rapidjson;
bool bAttached = false;
bool bSendUpdate = false;
ofstream trackerLogFile;
using easywsclient::WebSocket;
static WebSocket::pointer websocket = NULL;
deque<string> SendToServer;
vector<vector<string>> ShowWithBanner;
DWORD dwFrameCount = 0;
int framesToNextBanner = 120;
bool intro = false;

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

void Log(string message)
{
	if (trackerLogFile.is_open()) {
		trackerLogFile << "LOG: " << message << endl;
	}
}

void ParseMessages(const char* json, MessageMap& messages)
{
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
			//cout << "Action: " << itr->second << endl;
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

bool message_lock = false;
void push_message(string message)
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	try
	{
		SendToServer.push_back(message);
	}
	catch (int e){}
	message_lock = false;
}

string pop_message()
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	string message;
	try
	{
		message = SendToServer.front();
		cout << message.c_str() << endl;
		SendToServer.pop_front();
	}
	catch (int e){}
	message_lock = false;
	return message;
}

void clear_messages()
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	try
	{
		SendToServer.clear();
	}
	catch (int e){}
	message_lock = false;
}

DWORD WINAPI socketHandler(void *pThreadArgument)
{
	while (bAttached)
	{
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			Log("WSAStartup Failed.");
		}
#endif
		websocket = from_url(gIsaacUrl, false, gTrackerID);
		if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED) {
			Log("Websocket Could not Connect.");
		}
		else {
			Log("Websocket Connected.");
			while (websocket->getReadyState() != WebSocket::CLOSED && bAttached)
			{
				websocket->poll();
				websocket->dispatch(handle_message);
				if (SendToServer.size() > 0 && bSendUpdate)
				{
					websocket->send(pop_message());
					bSendUpdate = false;
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


PAPI VOID PostPlayer_Entity__AddCollectible(int ret)
{
	Player *pPlayer = GetPlayerEntity();
	if (!pPlayer){ return; }
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateItems");
		writer.String("items");
		writer.StartArray();
		for (int i = 0; i < 0x15A; i++)
		{
			if (pPlayer->_items[i])
				writer.Int(i);
		}
		writer.EndArray();
		writer.String("helditemid");
		writer.Int(pPlayer->_helditemid);
		writer.String("charges");
		writer.Int(pPlayer->_charges);
		writer.String("guppy");
		writer.Int(pPlayer->_nGuppyItems);
		writer.String("lof");
		writer.Int(pPlayer->_nFlyItems);
		writer.String("player_id");
		writer.Int(pPlayer->_charID);
		writer.String("luck");
		writer.Double(pPlayer->_luck);
		writer.String("damage");
		writer.Double(pPlayer->_damage);
		writer.String("range");
		writer.Double(pPlayer->_range);
		writer.String("shotheight");
		writer.Double(pPlayer->_shotheight);
		writer.String("shotspeed");
		writer.Double(pPlayer->_shotspeed);
		writer.String("tearrate");
		writer.Int(pPlayer->_tearrate);
		writer.String("speed");
		writer.Double(pPlayer->_speed);
	writer.EndObject();
	push_message(s.GetString());
}

int curKeys = 0;
PAPI VOID PostAddKeys(int ret)
{
	curKeys = ret;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateKeys");
		writer.String("keys");
		writer.Int(ret);
	writer.EndObject();
	push_message(s.GetString());
}

int curBombs = 0;
PAPI VOID PostAddBombs(int ret)
{
	curBombs = ret;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateBombs");
		writer.String("bombs");
		writer.Int(ret);
	writer.EndObject();
	push_message(s.GetString());
}

int curCoins = 0;
PAPI VOID PostAddCoins(int ret)
{
	curCoins = ret;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateCoins");
		writer.String("coins");
		writer.Int(ret);
	writer.EndObject();
	push_message(s.GetString());
}

int trinket1id = 0;
int trinket2id = 0;
PAPI VOID UpdateTrinkets(int id1, int id2)
{
	trinket1id = id1;
	trinket2id = id2;
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateTrinkets");
		writer.String("trinkets");
		writer.StartArray();
			writer.Int(id2);
			writer.Int(id1);
		writer.EndArray();
	writer.EndObject();
	push_message(s.GetString());
}

/* ret = boss id found in bossportraits.xml */
PAPI VOID PostTriggerBossDeath(int ret)
{

}

PAPI VOID PostLevel__Init(int ret)
{
	PlayerManager *pPlayerManager = GetPlayerManager();
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("updateFloor");
		writer.String("floor");
		writer.Int(pPlayerManager->m_Stage);
		writer.String("altfloor");
		writer.Int(pPlayerManager->m_AltStage);
		writer.String("curse");
		writer.Int(pPlayerManager->_curses);
	writer.EndObject();
	push_message(s.GetString());
}

PAPI VOID OnGame__Start(int challenge_id, bool disable_achievements, int player_id, char *seed, bool hard_mode)
{
	intro = true;
	bSendUpdate = true;
	dwFrameCount = 0;

	clear_messages();
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
		writer.String("action");
		writer.String("newGame");
		writer.String("challenge_id");
		writer.Int(challenge_id);
		writer.String("disable_achievements");
		writer.Int(disable_achievements);
		writer.String("player_id");
		writer.Int(player_id);
		writer.String("hard_mode");
		writer.Bool(hard_mode);
		writer.String("seed");
		writer.String(seed);
		writer.String("version");
		writer.Double(dTrackerVersion);
	writer.EndObject();
	push_message(s.GetString());
}

PAPI VOID PostGame__Start(int ret)
{
	Player *pPlayer = GetPlayerEntity();
	if (curBombs != pPlayer->_numBombs)
		PostAddBombs(pPlayer->_numBombs);
	if (curCoins != pPlayer->_numCoins)
		PostAddCoins(pPlayer->_numCoins);
	if (curKeys != pPlayer->_numKeys)
		PostAddKeys(pPlayer->_numKeys);
	if (trinket1id != pPlayer->_trinket1ID || trinket2id != pPlayer->_trinket2ID)
		UpdateTrinkets(pPlayer->_trinket1ID, pPlayer->_trinket2ID);
}

PAPI VOID OnGameUpdate()
{
	if (dwFrameCount >= 30 && !bSendUpdate)
	{
		bSendUpdate = true;
		dwFrameCount = 0;
	}
	if (dwFrameCount >= 60 * 10)
	{
		Player *pPlayer = GetPlayerEntity();
		if (curBombs != pPlayer->_numBombs)
			PostAddBombs(pPlayer->_numBombs);
		if (curCoins != pPlayer->_numCoins)
			PostAddCoins(pPlayer->_numCoins);
		if (curKeys != pPlayer->_numKeys)
			PostAddKeys(pPlayer->_numKeys);
		if (trinket1id != pPlayer->_trinket1ID || trinket2id != pPlayer->_trinket2ID)
			UpdateTrinkets(pPlayer->_trinket1ID, pPlayer->_trinket2ID);
		dwFrameCount = 0;
	}
	if (dwFrameCount >= (60 * 3) && intro)
	{
		intro = false;
		show_fortune_banner("", "isaactracker loaded!", "");
	}
	if (!ShowWithBanner.empty() && framesToNextBanner <= 0){
		show_fortune_banner(ShowWithBanner.back().at(0).c_str(), ShowWithBanner.back().at(1).c_str(), ShowWithBanner.back().at(2).c_str());
		ShowWithBanner.pop_back();
		framesToNextBanner = 60;
	}
	dwFrameCount++;
	framesToNextBanner--;
}
