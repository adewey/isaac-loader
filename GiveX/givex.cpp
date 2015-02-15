#include "..\loader\plugin.h"

void GiveX(int argc, char *argv[])
{
	string thingToGive = (string)argv[0];
	for (string::size_type i = 0; i < thingToGive.length(); ++i){ //all Lowercase.
		thingToGive[i] = tolower(thingToGive[i]);
	}
	for (int i = 1; i < argc; i++){
		string value = argv[i];
		cout << argv[0] << ": " << atos(value) << endl;
	}
	//if (newValue.find_first_not_of("1234567890.-") != string::npos){  //This is Not a Real Number, inform the user.
	//	std::cout << "Not a Real Number" << endl;
	//}
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