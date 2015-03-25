#include "plugins.h"
#include "utilities.h"
#include "commands.h"
char gszPluginPath[MAX_PATH] = { 0 };
char gszINIPath[MAX_PATH] = { 0 };
PPLUGIN pPluginList = 0;

bool LoadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };

	strcpy_s(Filename, MAX_PATH, fn);
	_strlwr_s(Filename, MAX_PATH);
	char *Temp = strstr(Filename, ".dll");
	if (Temp)
		Temp[0] = 0;

	char TheFilename[MAX_STRING] = { 0 };
	sprintf_s(TheFilename, MAX_STRING, "%s.dll", Filename);

	char FullFilename[MAX_STRING] = { 0 };
	sprintf_s(FullFilename, MAX_STRING, "%s\\%s.dll", gszPluginPath, Filename);

	HMODULE hmod = LoadLibraryA(FullFilename);
	if (!hmod)
	{
		/* plugin failed to load */
		return false;
	}

	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (hmod == pPlugin->hModule)
		{
			/* plugin already loaded*/
			FreeLibrary(hmod);
			return true;
		}
		pPlugin = pPlugin->pNext;
	}

	pPlugin = new PLUGIN;
	memset(pPlugin, 0, sizeof(PLUGIN));
	pPlugin->hModule = hmod;
	strcpy_s(pPlugin->szPluginName, MAX_PATH, Filename);

	pPlugin->InitPlugin = (fInitPlugin)GetProcAddress(hmod, "InitPlugin");
	pPlugin->UnInitPlugin = (fUnInitPlugin)GetProcAddress(hmod, "UnInitPlugin");

	pPlugin->PreGame__Start = (fPreGame__Start)GetProcAddress(hmod, "PreGame__Start");
	pPlugin->OnGame__Start = (fOnGame__Start)GetProcAddress(hmod, "OnGame__Start");
	pPlugin->PostGame__Start = (fPostGame__Start)GetProcAddress(hmod, "PostGame__Start");

	pPlugin->PrePlayer_Entity__AddCollectible = (fPrePlayer_Entity__AddCollectible)GetProcAddress(hmod, "PrePlayer_Entity__AddCollectible");
	pPlugin->OnPlayer_Entity__AddCollectible = (fOnPlayer_Entity__AddCollectible)GetProcAddress(hmod, "OnPlayer_Entity__AddCollectible");
	pPlugin->PostPlayer_Entity__AddCollectible = (fPostPlayer_Entity__AddCollectible)GetProcAddress(hmod, "PostPlayer_Entity__AddCollectible");

	pPlugin->PostTriggerBossDeath = (fPostTriggerBossDeath)GetProcAddress(hmod, "PostTriggerBossDeath");

	pPlugin->OnLevel__Init = (fOnLevel__Init)GetProcAddress(hmod, "OnLevel__Init");
	pPlugin->PostLevel__Init = (fPostLevel__Init)GetProcAddress(hmod, "PostLevel__Init");

	pPlugin->PreItemPool__GetCollectible = (fPreItemPool__GetCollectible)GetProcAddress(hmod, "PreItemPool__GetCollectible");
	pPlugin->PostItemPool__GetCollectible = (fPostItemPool__GetCollectible)GetProcAddress(hmod, "PostItemPool__GetCollectible");

	pPlugin->OnGameUpdate = (fOnGameUpdate)GetProcAddress(hmod, "OnGameUpdate");

	pPlugin->OnReceiveMessage = (fOnReceiveMessage)GetProcAddress(hmod, "OnReceiveMessage");

	

	if (pPlugin->InitPlugin)
		pPlugin->InitPlugin();

	pPlugin->pLast = 0;
	pPlugin->pNext = pPluginList;
	if (pPluginList)
		pPluginList->pLast = pPlugin;
	pPluginList = pPlugin;
	WritePrivateProfileStringA("plugins", Filename, Filename, gszINIPath);
	return true;
}

bool UnloadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };
	strcpy_s(Filename, MAX_PATH, fn);
	_strlwr_s(Filename, MAX_PATH);
	char *Temp = strstr(Filename, ".dll");
	if (Temp)
		Temp[0] = 0;

	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (!_stricmp(Filename, pPlugin->szPluginName))
		{
			if (pPlugin->pLast)
				pPlugin->pLast->pNext = pPlugin->pNext;
			else
				pPluginList = pPlugin->pNext;
			if (pPlugin->pNext)
				pPlugin->pNext->pLast = pPlugin->pLast;

			if (pPlugin->UnInitPlugin)
				pPlugin->UnInitPlugin();

			FreeLibrary(pPlugin->hModule);
			delete pPlugin;
			/* make sure it isnt loaded by default next time */
			WritePrivateProfileStringA("plugins", Filename, NULL, gszINIPath);
			return true;
		}
		pPlugin = pPlugin->pNext;
	}

	return false;
}

void UnloadPlugins()
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		UnloadPlugin(pPlugin->szPluginName);
	}
}

/* NOTE(Aaron): could some or all of this function be moved to commands? i'm not 100% */
void InitPlugins()
{
	char szPlugins[MAX_STRING] = { 0 };
	/* get a null terminated string of keys in the plugin directory*/
	DWORD dwBytes = IniReadString("plugins", NULL, szPlugins); // GetPrivateProfileStringA("plugins", NULL, NULL, szPlugins, MAX_STRING, gszINIPath);

	/* convert this to something meaningful to parse */
	for (DWORD i = 0; i < dwBytes; i++)
		if (szPlugins[i] == '\0' && szPlugins[i + 1] != '\0')
			szPlugins[i] = ' ';

	/* get an array of plugin strings to load */
	PCHAR *szPluginList;
	int nPlugins;
	szPluginList = CommandLineToArgvA(szPlugins, &nPlugins);
	
	/* load them */
	for (int o = 0; o < nPlugins; o++)
		LoadPlugin(szPluginList[o]);
}



void PreGame__Start(int *challenge_id, bool *disable_achievements, int *character_id, char *seed, bool *hard_mode)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PreGame__Start){
			pPlugin->PreGame__Start(challenge_id, disable_achievements, character_id, seed, hard_mode);
		}
		pPlugin = pPlugin->pNext;
	}
}

void OnGame__Start(int challenge_id, bool disable_achievements, int character_id, char *seed, bool hard_mode)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnGame__Start){
			pPlugin->OnGame__Start(challenge_id, disable_achievements, character_id, seed, hard_mode);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostGame__Start(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostGame__Start){
			pPlugin->PostGame__Start(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}



void PrePlayer_Entity__AddCollectible(Player *pPlayer, int *item_id, int *charges)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PrePlayer_Entity__AddCollectible){
			pPlugin->PrePlayer_Entity__AddCollectible(pPlayer, item_id, charges);
		}
		pPlugin = pPlugin->pNext;
	}
}

void OnPlayer_Entity__AddCollectible(Player *pPlayer, int item_id, int charges)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnPlayer_Entity__AddCollectible){
			pPlugin->OnPlayer_Entity__AddCollectible(pPlayer, item_id, charges);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostPlayer_Entity__AddCollectible(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostPlayer_Entity__AddCollectible){
			pPlugin->PostPlayer_Entity__AddCollectible(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}



void PostTriggerBossDeath(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostTriggerBossDeath){
			pPlugin->PostTriggerBossDeath(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}


void OnLevel__Init(int level)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnLevel__Init){
			pPlugin->OnLevel__Init(level);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostLevel__Init(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostLevel__Init){
			pPlugin->PostLevel__Init(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}



bool PreItemPool__GetCollectible(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PreItemPool__GetCollectible){
			if (!pPlugin->PreItemPool__GetCollectible(id))
				ret = false;
		}
		pPlugin = pPlugin->pNext;
	}
	return ret;
}

void PostItemPool__GetCollectible(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PostItemPool__GetCollectible){
			pPlugin->PostItemPool__GetCollectible(id);
		}
		pPlugin = pPlugin->pNext;
	}
	return;
}



bool PreItemPool__GetCard(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PreItemPool__GetCard){
			if (!pPlugin->PreItemPool__GetCard(id))
				ret = false;
		}
		pPlugin = pPlugin->pNext;
	}
	return ret;
}

void PostItemPool__GetCard(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PostItemPool__GetCard){
			pPlugin->PostItemPool__GetCard(id);
		}
		pPlugin = pPlugin->pNext;
	}
	return;
}



bool PreItemPool__GetRune(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PreItemPool__GetRune){
			if (!pPlugin->PreItemPool__GetRune(id))
				ret = false;
		}
		pPlugin = pPlugin->pNext;
	}
	return ret;
}

void PostItemPool__GetRune(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PostItemPool__GetRune){
			pPlugin->PostItemPool__GetRune(id);
		}
		pPlugin = pPlugin->pNext;
	}
	return;
}



void OnGameUpdate()
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnGameUpdate)
			pPlugin->OnGameUpdate();
		pPlugin = pPlugin->pNext;
	}
}


void OnReceiveMessage(MessageMap messages)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnReceiveMessage)
			pPlugin->OnReceiveMessage(messages);
		pPlugin = pPlugin->pNext;
	}
}
