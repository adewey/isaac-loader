#include "utilities.h"
#include "commands.h" 
#include "hooks.h"
#include "plugins.h"
#include "statics.h"
#include "sockets.h"

HANDLE ghThread;
bool gbAttached = false;

#ifdef _DEBUG
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

#include <fstream>
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	std::ofstream f;
	f.open("gemini\\VectoredExceptionHandler.txt", std::ios::out | std::ios::trunc);
	f << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << std::endl;
	f << std::hex << pExceptionInfo->ExceptionRecord->ExceptionAddress << std::endl;
	f.close();

	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	std::ofstream f;
	f.open("gemini\\TopLevelExceptionHandler.txt", std::ios::out | std::ios::trunc);
	f << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << std::endl;
	f << std::hex << pExceptionInfo->ExceptionRecord->ExceptionAddress << std::endl;
	f.close();

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif


bool VerifyIsaac()
{
	if (!_stricmp(GetIsaacVersion(), "Binding of Isaac: Rebirth v1.05"))
		return false;
	return true;
}


DWORD WINAPI DllThread(void* pThreadArgument)
{
	InitStatics();
	if (!VerifyIsaac())
	{
		gbAttached = false;
		MessageBoxA(NULL, "Isaac has been updated and gemini modloader has not. If there isn't an update posted on the website (isaactracker.com), there will be shortly.", "gemini modloader unable to load", 0);
		return 0;
	}

#ifdef _DEBUG
	/* attach console and hooks */
	InitConsole();

	AddVectoredExceptionHandler(1, VectoredExceptionHandler);
	SetUnhandledExceptionFilter(TopLevelExceptionHandler);
#endif

	InitHooks();

	/* set our plugin path to the directory our main dll was loaded from */
	strcpy_s(gszPluginPath, MAX_PATH, (char *)pThreadArgument);
	sprintf_s(gszJSONPath, MAX_PATH, "%s\\settings.json", gszPluginPath);

	InitPlugins();
	InitSockets();
	
#ifdef _DEBUG
	cout << GetIsaacVersion() << endl;
	/* add commands */
	AddCommand("load", PluginLoad);
	AddCommand("unload", PluginUnload);
	AddCommand("detatch", detatch);
#endif

	/* wait to be detached */
	while (gbAttached) {
#ifdef _DEBUG
		/* monitor for commands */
		char buffer[MAX_PATH] = { 0 };
		fgets(buffer, MAX_PATH, stdin);
		ParseCommand(buffer);
#else
		Sleep(100);
#endif
	}
	
	UnInitSockets();
	/* unload our plugins */
	UnInitPlugins();

#ifdef _DEBUG
	/* remove commands*/
	RemoveCommand("unload");
#endif

	/* remove console and unhook */
	UnInitHooks();

#ifdef _DEBUG
	cout << "DLL Detached!" << endl;
	UnInitConsole();
#endif

	Sleep(1000);
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
