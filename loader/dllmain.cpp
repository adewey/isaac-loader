#include "utilities.h"
#include "commands.h"
#include "events.h"
#include "hooks.h"
#include "statics.h"

bool gbAttached = false;
HANDLE ghThread;
char *gTrackerID = "64a3c29a-c71b-4b35-b08c-38d3f5a70586";
Player * gpPlayer = 0;

void setKey(Player *pPlayer, int argc, char *argv[])
{
	gTrackerID = argv[0];
	cout << "Tracker ID set to: " << gTrackerID << endl;
}

void getKey(Player *pPlayer, int argc, char *argv[])
{
	cout << "Your current tracker ID: " << gTrackerID << endl;
}

void unLoad(Player *pPlayer, int argc, char*argv[])
{
	cout << "Unloading..." << endl;
	gbAttached = false;
}

void setCurse(Player *pPlayer, int argc, char *argv[]){
	GetPlayerManager()->_curses = atoi(argv[0]);
}
DWORD WINAPI DllThread(void* pThreadArgument)
{
	/* attach console and hooks */
	InitConsole();
	cout << "DLL Attached!" << endl;
	InitHooks();

	/* add commands */
	AddCommand("unload", unLoad);
	AddCommand("setkey", setKey);
	AddCommand("getkey", getKey);
	AddCommand("setcurse", setCurse);
	/* wait to be detached */
	while (gbAttached){
		/* monitor for commands */
		char buffer[MAX_PATH] = { 0 };
		fgets(buffer, MAX_PATH, stdin);
		ParseCommand(buffer);
	}
	
	/* remove commands*/
	RemoveCommand("unload");
	RemoveCommand("setkey");
	RemoveCommand("getkey");
	RemoveCommand("setcurse");

	/* remove console and unhook */
	cout << "DLL Detached!" << endl;
	RemoveConsole();
	RemoveHooks();
	return 0;
}


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		/* use a global attached variable and a thread to make sure we don't crash if isaac is unloaded from under us */
		if (!gbAttached)
		{
			gbAttached = true;
			//everything should be done in a thread instead of as part of the process attach call 
			ghThread = CreateThread(NULL, 0, DllThread, NULL, 0L, NULL);
		}
		break;
	case DLL_PROCESS_DETACH:
		gbAttached = false;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}
