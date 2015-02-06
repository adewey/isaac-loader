#include "utilities.h"
#include "commands.h"
#include "events.h"
#include "hooks.h"
#include "plugins.h"
#include "statics.h"

HANDLE ghThread;
bool gbAttached = false;

void detatch(int argc, char *argv[])
{
	cout << "detatching..." << endl;
	gbAttached = false;
}

void PluginLoad(int argc, char *argv[])
{
	if (!argc) return;
	char *loaded = (LoadPlugin(argv[0])) ? "success" : "failed";
	cout << "loading plugin " << argv[0] << "... " << loaded << endl;
}

void PluginUnload(int argc, char *argv[])
{
	if (!argc) return;
	char *unloaded = (UnloadPlugin(argv[0])) ? "success" : "failed";
	cout << "unloading plugin " << argv[0] << "... " << unloaded << endl;
}

void setCurse(int argc, char *argv[])
{
	PPLAYERMANAGER pPlayerManager = GetPlayerManager();
	cout << "pPlayerManager [0x" << (void *)pPlayerManager << "]" << endl;
	pPlayerManager->i_curses = atoi(argv[0]);
}

DWORD WINAPI DllThread(void* pThreadArgument)
{
	/* attach console and hooks */
	InitConsole();
	cout << "DLL Attached!" << endl;
	InitHooks();

	/* set our plugin path to the directory our main dll was loaded from */
	strcpy_s(gszPluginPath, MAX_PATH, (char *)pThreadArgument);

	/* add commands */
	AddCommand("load", PluginLoad);
	AddCommand("unload", PluginUnload);
	AddCommand("detatch", detatch);
	AddCommand("setcurse", setCurse);

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
	RemoveCommand("setcurse");

	/* remove console and unhook */
	cout << "DLL Detached!" << endl;
	RemoveConsole();
	RemoveHooks();
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
			ghThread = CreateThread(NULL, 0, DllThread, _strdup(szPath), 0L, NULL);
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
