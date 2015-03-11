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
GLOBAL typedef void(__cdecl *fPostTriggerBossDeath)(int);
GLOBAL typedef void(__cdecl *fPostLevel__Init)(int);
GLOBAL typedef bool(__cdecl *fPostEntity_Pickup__Init)(int);
GLOBAL typedef bool(__cdecl *fPostItemPool__GetCollectible)(int);
GLOBAL typedef bool(__cdecl *fPostEntity_Shop_Pickup__Init)(int);
GLOBAL typedef void(__cdecl *fOnEntity_Pickup__Morph)(Entity *, int, int, int, BOOL);
GLOBAL typedef void(__cdecl *fPostEntity_Pickup__Morph)(int);
GLOBAL typedef void(__cdecl *fOnGameUpdate)();

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fPostStartGame PostStartGame;
	fPostAddCollectible PostAddCollectible;
	fPostTriggerBossDeath PostTriggerBossDeath;
	fPostLevel__Init PostLevel__Init;
	fPostEntity_Pickup__Init PostEntity_Pickup__Init;
	fPostItemPool__GetCollectible PostItemPool__GetCollectible;
	fPostEntity_Pickup__Init PostEntity_Shop_Pickup__Init;
	fOnEntity_Pickup__Morph OnEntity_Pickup__Morph;
	fPostEntity_Pickup__Morph PostEntity_Pickup__Morph;
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
GLOBAL void PostTriggerBossDeath(int);
GLOBAL void PostLevel__Init(int);
GLOBAL bool PostEntity_Pickup__Init(int);
GLOBAL bool PostItemPool__GetCollectible(int);
GLOBAL bool PostEntity_Shop_Pickup__Init(int);
GLOBAL void OnEntity_Pickup__Morph(Entity *pEntity, int id, int variant, int subtype, BOOL unknown);
GLOBAL void PostEntity_Pickup__Morph(int);
GLOBAL void OnGameUpdate();

#endif
