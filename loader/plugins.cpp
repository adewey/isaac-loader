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
	pPlugin->PostAddCollectible = (fPostAddCollectible)GetProcAddress(hmod, "PostAddCollectible");
	pPlugin->PostChangeKeys = (fPostChangeKeys)GetProcAddress(hmod, "PostChangeKeys");
	pPlugin->PostChangeBombs = (fPostChangeBombs)GetProcAddress(hmod, "PostChangeBombs");
	pPlugin->PostChangeCoins = (fPostChangeCoins)GetProcAddress(hmod, "PostChangeCoins");
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

void PostChangeKeys(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostChangeKeys){
			pPlugin->PostChangeKeys(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostChangeBombs(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostChangeBombs){
			pPlugin->PostChangeBombs(ret);
		}
		pPlugin = pPlugin->pNext;
	}
}

void PostChangeCoins(int ret)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (pPlugin->PostChangeCoins){
			pPlugin->PostChangeCoins(ret);
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
