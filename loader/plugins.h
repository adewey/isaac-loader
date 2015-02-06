#ifndef PLUGINS_H
#define PLUGINS_H

#include "statics.h"
#include "utilities.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL char gszINIPath[MAX_PATH];
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fOnAddCollectible)(PPLAYER, int, int, int, int);
GLOBAL typedef void(__cdecl *fOnGameUpdate)();

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fOnAddCollectible OnAddCollectible;
	fOnGameUpdate OnGameUpdate;

	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

GLOBAL PPLUGIN pPluginList;

GLOBAL bool LoadPlugin(const char *);
GLOBAL bool UnloadPlugin(const char *);
GLOBAL void UnloadPlugins();
GLOBAL void InitPlugins();

GLOBAL void OnAddCollectible(PPLAYER, int, int, int, int);
GLOBAL void OnGameUpdate();

#endif
