#include "..\loader\plugin.h"

void TemplateCommand(int argc, char *argv[])
{
	//argc = arg count
	//argv = array of args
}

// called when the plugin initializes
PAPI VOID InitPlugin()
{
	//Add commands, detours, etc
	AddCommand("templatecommand", TemplateCommand);
}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	//remove commands, detours, etc
	RemoveCommand("templatecommand");
}

PAPI VOID OnAddCollectible(PPLAYER pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	//do stuff with the collectible's information
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