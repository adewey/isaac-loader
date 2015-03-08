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
	pPlugin->PostStartGame = (fPostStartGame)GetProcAddress(hmod, "PostStartGame");
	pPlugin->PostAddCollectible = (fPostAddCollectible)GetProcAddress(hmod, "PostAddCollectible");
	pPlugin->PostAddKeys = (fPostAddKeys)GetProcAddress(hmod, "PostAddKeys");
	pPlugin->PostAddBombs = (fPostAddBombs)GetProcAddress(hmod, "PostAddBombs");
	pPlugin->PostAddCoins = (fPostAddCoins)GetProcAddress(hmod, "PostAddCoins");
	pPlugin->PostTriggerBossDeath = (fPostTriggerBossDeath)GetProcAddress(hmod, "PostTriggerBossDeath");
	pPlugin->PostLevel__Init = (fPostLevel__Init)GetProcAddress(hmod, "PostLevel__Init");
	pPlugin->PostEntity_Pickup__Init = (fPostEntity_Pickup__Init)GetProcAddress(hmod, "PostEntity_Pickup__Init");
	pPlugin->PostEntity_Shop_Pickup__Init = (fPostEntity_Shop_Pickup__Init)GetProcAddress(hmod, "PostEntity_Shop_Pickup__Init");
	pPlugin->OnEntity_Pickup__Morph = (fOnEntity_Pickup__Morph)GetProcAddress(hmod, "OnEntity_Pickup__Morph");
	pPlugin->PostEntity_Pickup__Morph = (fPostEntity_Pickup__Morph)GetProcAddress(hmod, "PostEntity_Pickup__Morph");
	pPlugin->OnGameUpdate = (fOnGameUpdate)GetProcAddress(hmod, "OnGameUpdate");

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

void PostStartGame(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostStartGame){
			pPlugin->PostStartGame(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostAddCollectible(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostAddCollectible){
			pPlugin->PostAddCollectible(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostAddKeys(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostAddKeys){
			pPlugin->PostAddKeys(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostAddBombs(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostAddBombs){
			pPlugin->PostAddBombs(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostAddCoins(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostAddCoins){
			pPlugin->PostAddCoins(ret);
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

bool PostEntity_Pickup__Init(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PostEntity_Pickup__Init){
			if (!pPlugin->PostEntity_Pickup__Init(id))
				ret = false;
		}
		pPlugin = pPlugin->pNext;
	}
	return ret;
}

bool PostEntity_Shop_Pickup__Init(int id)
{
	PPLUGIN pPlugin = pPluginList;
	bool ret = true;
	while (pPlugin)
	{
		if (pPlugin->PostEntity_Shop_Pickup__Init){
			if (!pPlugin->PostEntity_Shop_Pickup__Init(id))
				ret = false;
		}
		pPlugin = pPlugin->pNext;
	}
	return ret;
}

void OnEntity_Pickup__Morph(Entity *pEntity, int id, int variant, int subtype, BOOL unknown)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->OnEntity_Pickup__Morph)
			pPlugin->OnEntity_Pickup__Morph(pEntity, id, variant, subtype, unknown);
		pPlugin = pPlugin->pNext;
	}
}

void PostEntity_Pickup__Morph(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostEntity_Pickup__Morph){
			pPlugin->PostEntity_Pickup__Morph(ret);
		}
		pPlugin = pPlugin->pNext;
	}
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
