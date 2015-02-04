#include "plugins.h"
#include "utilities.h"

DWORD LoadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };

	strcpy(Filename, fn);
	strlwr(Filename);
	char *Temp = strstr(Filename, ".dll");
	if (Temp)
		Temp[0] = 0;

	char TheFilename[MAX_STRING] = { 0 };
	sprintf(TheFilename, "%s.dll", Filename);


	char FullFilename[MAX_STRING] = { 0 };
//	sprintf(FullFilename, "%s\\%s.dll", gszINIPath, Filename);

	HMODULE hmod = LoadLibraryA(FullFilename);
	if (!hmod)
	{
		/* plugin failed to load */
		return 0;
	}

	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (hmod == pPlugin->hModule)
		{
			/* plugin already loaded*/
			FreeLibrary(hmod);
			return 2;
		}
		pPlugin = pPlugin->pNext;
	}

	pPlugin = new PLUGIN;
	memset(pPlugin, 0, sizeof(PLUGIN));
	pPlugin->hModule = hmod;
	strcpy(pPlugin->szFilename, Filename);

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
	return 1;
}

bool UnloadPlugin(const char *fn)
{
	char Filename[MAX_PATH] = { 0 };
	strcpy(Filename, fn);
	strlwr(Filename);
	char *Temp = strstr(Filename, ".dll");
	if (Temp)
		Temp[0] = 0;

	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		if (!stricmp(Filename, pPlugin->szFilename))
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
			return 1;
		}
		pPlugin = pPlugin->pNext;
	}

	return 0;
}

void PluginsOnEventExample()
{
	PPLUGIN pPlugin = pPluginList;
	while (pPlugin)
	{
		pPlugin->OnEventExample();
		pPlugin = pPlugin->pNext;
	}
}
