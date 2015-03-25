#include "..\loader\plugin.h"
#include <fstream>

using namespace rapidjson;
using namespace std;
PWEBSOCKET isaactracker;

double dTrackerVersion = 1.31;

char gTrackerID[MAX_STRING] = { 0 };
char *gIsaacUrl = "ws://ws.isaactracker.com";

ofstream trackerLogFile;
DWORD dwFrameCount = 0;
int framesToNextBanner = 120;
bool intro = false;

vector<vector<string>> ShowWithBanner;

void Log(string message)
{
	if (trackerLogFile.is_open()) {
		trackerLogFile << "LOG: " << message << endl;
	}
}

PAPI VOID InitPlugin()
{
	trackerLogFile.open("gemini/tracker_log.txt");
	Log("Tracker Started");
	IniReadString("tracker", "key", gTrackerID);
	string trackerLog = "Tracker ID: ";
	trackerLog.append(gTrackerID);
	Log(trackerLog);
	isaactracker = AddSocket(gIsaacUrl, gTrackerID);
}

PAPI VOID UnInitPlugin(VOID)
{
	RemoveSocket(isaactracker);
	trackerLogFile.close();
}


PAPI VOID OnReceiveMessage(MessageMap messages)
{
	string action;
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
	push_message(isaactracker, s.GetString());
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
	push_message(isaactracker, s.GetString());
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
	push_message(isaactracker, s.GetString());
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
	push_message(isaactracker, s.GetString());
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
	push_message(isaactracker, s.GetString());
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
	push_message(isaactracker, s.GetString());
}

PAPI VOID OnGame__Start(int challenge_id, bool disable_achievements, int player_id, char *seed, bool hard_mode)
{
	intro = true;
	isaactracker->send_update = true;
	dwFrameCount = 0;

	clear_messages(isaactracker);
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
	push_message(isaactracker, s.GetString());
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
	if (dwFrameCount >= 30 && !isaactracker->send_update)
	{
		isaactracker->send_update = true;
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
