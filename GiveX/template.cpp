#include "..\loader\plugin.h"

void GiveX(int argc, char *argv[])
{
	string thingToGive = argv[0];

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