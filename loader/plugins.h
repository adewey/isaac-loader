#ifndef PLUGINS_H
#define PLUGINS_H

#include "statics.h"
#include "utilities.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL char gszINIPath[MAX_PATH];
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fPostStartGame)(int);
GLOBAL typedef void(__cdecl *fPostAddCollectible)(int);
GLOBAL typedef void(__cdecl *fPostAddKeys)(int);
GLOBAL typedef void(__cdecl *fPostAddBombs)(int);
GLOBAL typedef void(__cdecl *fPostAddCoins)(int);
GLOBAL typedef void(__cdecl *fPostTriggerBossDeath)(int);
GLOBAL typedef void(__cdecl *fOnGameUpdate)();

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fPostStartGame PostStartGame;
	fPostAddCollectible PostAddCollectible;
	fPostAddKeys PostAddKeys;
	fPostAddBombs PostAddBombs;
	fPostAddCoins PostAddCoins;
	fPostTriggerBossDeath PostTriggerBossDeath;
	fOnGameUpdate OnGameUpdate;

	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

GLOBAL PPLUGIN pPluginList;

GLOBAL bool LoadPlugin(const char *);
GLOBAL bool UnloadPlugin(const char *);
GLOBAL void UnloadPlugins();
GLOBAL void InitPlugins();

GLOBAL void PostStartGame(int);
GLOBAL void PostAddCollectible(int);
GLOBAL void PostAddKeys(int);
GLOBAL void PostAddBombs(int);
GLOBAL void PostAddCoins(int);
GLOBAL void PostTriggerBossDeath(int);
GLOBAL void OnGameUpdate();

#endif
