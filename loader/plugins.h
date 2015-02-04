#ifndef PLUGINS_H
#define PLUGINS_H

#include "isaacdata.h"

typedef struct _Plugin
{
	char szFilename[MAX_PATH];
	HMODULE hModule;

	/* plugin event hooks */
	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fOnAddCollectible OnAddCollectible;

	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

PPLUGIN pPluginList;
GLOBAL char *gszPluginPath;
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fOnAddCollectible)(Player* pPlayer, int relatedID, int itemID, int charges, int arg5);

void UnloadPlugins();

#endif
