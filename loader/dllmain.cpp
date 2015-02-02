#include "utilities.h"
#include "commands.h"
#include "events.h"
#include "hooks.h"

bool gbAttached = false;
HANDLE ghThread;

DWORD WINAPI DllThread(void* pThreadArgument)
{
	InitConsole();
	cout << "DLL Attached!\n";
	InitHooks();

	/* wait to be detached */
	while (gbAttached){
		/* monitor for commands */
		char buffer[MAX_PATH] = { 0 };
		cin >> buffer;
		if (strstr(buffer, "setkey ") != NULL){
			cout << "Command Recognized. \n";
			char * keyLoc = strstr(buffer, "setkey ") + 7; // 7 is length of 'setkey '
			char *newline;
			if ((newline = strchr(keyLoc, '\n')) != NULL){
				*newline = '\0';
			}
			SetTrackerID(keyLoc);
			cout << "Tracker ID set to: '" << GetTrackerID() << "';" << endl;
		}
		else{
			cout << "Not a command. \n";
		}
		cout << "dll[" << buffer << "]\n";
	}

	RemoveConsole();
	RemoveHooks();
	cout << "DLL Detached!\n";
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
