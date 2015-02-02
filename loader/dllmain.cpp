#include "utilities.h"
#include "commands.h"
#include "events.h"
#include "hooks.h"
using namespace std;

bool gbAttached = false;
HANDLE ghThread;

DWORD WINAPI DllThread(void* pThreadArgument)
{
	InitConsole();
	printf("DLL Attached!\n");
	InitHooks();

	/* wait to be detached */
	while (gbAttached){ 
		/* monitor for commands */
		char buffer[MAX_PATH];
		fgets(buffer, MAX_PATH, stdin);
		
	}
	RemoveConsole();
	RemoveHooks();
	printf("DLL Detached!\n");
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
