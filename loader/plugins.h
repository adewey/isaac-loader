#ifndef PLUGINS_H
#define PLUGINS_H

#include "isaacdata.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fOnAddCollectible)(Player*, int, int, int, int);

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;
	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fOnAddCollectible OnAddCollectible;
	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

GLOBAL PPLUGIN pPluginList;

GLOBAL bool LoadPlugin(const char *);
GLOBAL bool UnloadPlugin(const char *);
GLOBAL void UnloadPlugins();

GLOBAL void AddCollectible(Player*, int, int, int, int);

#endif
