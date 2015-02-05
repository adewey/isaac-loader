#include "plugins.h"
#include "utilities.h"

char gszPluginPath[MAX_PATH] = { 0 };
PPLUGIN pPluginList = 0;

bool LoadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };

	strcpy_s(Filename, MAX_PATH, fn);
	strlwr(Filename);
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
	pPlugin->OnAddCollectible = (fOnAddCollectible)GetProcAddress(hmod, "OnAddCollectible");

	if (pPlugin->InitPlugin)
		pPlugin->InitPlugin();

	pPlugin->pLast = 0;
	pPlugin->pNext = pPluginList;
	if (pPluginList)
		pPluginList->pLast = pPlugin;
	pPluginList = pPlugin;
	return true;
}

bool UnloadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };
	strcpy_s(Filename, MAX_PATH, fn);
	strlwr(Filename);
	char *Temp = strstr(Filename, ".dll");
	if (Temp)
		Temp[0] = 0;

	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (!stricmp(Filename, pPlugin->szPluginName))
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

void AddCollectible(Player* pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		pPlugin->OnAddCollectible(pPlayer, relatedID, itemID, charges, arg5);
		pPlugin = pPlugin->pNext;
	}
}
