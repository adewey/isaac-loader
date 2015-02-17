#include "..\loader\plugin.h"
#include <string>

void GiveX(int argc, char *argv[])
{
	string thingToGive = argv[0];
	for (string::size_type i = 0; i < thingToGive.length(); ++i){ //all Lowercase.
		thingToGive[i] = tolower(thingToGive[i]);
	}
	for (int i = 1; i < argc; i++){
		string value = argv[i];
		if (value.find_first_not_of("1234567890") == string::npos){
			if (thingToGive == "item"){
				AddCollectible(GetPlayerEntity(), stoi(value));
			}
			else if (thingToGive == "key" || thingToGive == "keys"){
				GetPlayerEntity()->_numKeys += stoi(value);
			}
			else if (thingToGive == "goldkey"){
				GetPlayerEntity()->_hasGoldenKey = stoi(value);
			}
			else if (thingToGive == "bomb" || thingToGive == "bombs"){
				GetPlayerEntity()->_numBombs += stoi(value);
			}
			else if (thingToGive == "coin" || thingToGive == "coins"){
				GetPlayerEntity()->_numCoins += stoi(value);
			}
			else{
				cout << "'" << thingToGive << "' is not a thing. (item, key, goldkey, bomb, coin)" << endl;
			}
		}
	}
}

// called when the plugin initializes
PAPI VOID InitPlugin()
{
	//Add commands, detours, etc
	AddCommand("give", GiveX);
}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	//remove commands, detours, etc
	RemoveCommand("give");
}

DWORD dwFrameCount = 0;
PAPI VOID OnGameUpdate()
{
	//if you are going to do stuff in OnGameUpdate you should probably only do it once every 60 frames (1 sec at 60fps)
	if (dwFrameCount > 60 * 60)
	{
		//here is an example of doing something every 60*60 frames (once per minute)
		dwFrameCount = 0;
	}
	dwFrameCount++;
}