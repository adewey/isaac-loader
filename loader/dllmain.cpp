#include "utilities.h"
#include "commands.h"
#include "events.h"
#include "hooks.h"
#include "plugins.h"

bool gbAttached = false;
HANDLE ghThread;
Player * gpPlayer = 0;

void unLoad(Player *pPlayer, int argc, char*argv[])
{
	cout << "Unloading..." << endl;
	gbAttached = false;
}

DWORD WINAPI DllThread(void* pThreadArgument)
{
	/* set our plugin path to the directory our main dll was loaded from */
	strcpy(gszPluginPath, (char *)pThreadArgument);
	/* attach console and hooks */
	InitConsole();
	cout << "DLL Attached!" << endl;
	InitHooks();

	/* add commands */
	AddCommand("unload", unLoad);

	/* wait to be detached */
	while (gbAttached){
		/* monitor for commands */
		char buffer[MAX_PATH] = { 0 };
		fgets(buffer, MAX_PATH, stdin);
		ParseCommand(buffer);
	}
	
	/* unload our plugins */
	UnloadPlugins();

	/* remove commands*/
	RemoveCommand("unload");

	/* remove console and unhook */
	RemoveConsole();
	RemoveHooks();
	cout << "DLL Detached!" << endl;
	return 0;
}


int APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		/* use a global attached variable and a thread to make sure we don't crash if isaac is unloaded from under us */
		if (!gbAttached)
		{
			/* get the directory we were loaded from, so that we can load other things */
			char szPath[MAX_PATH] = { 0 };
			char * szTempPath;
			GetModuleFileNameA(hDLL, szPath, MAX_STRING);
			szTempPath = strrchr(szPath, '\\');
			szTempPath[1] = '\0';

			/* everything should be done in a thread instead of as part of the process attach call */
			gbAttached = true;
			ghThread = CreateThread(NULL, 0, DllThread, strdup(szPath), 0L, NULL);
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
