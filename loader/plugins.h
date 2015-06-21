#ifndef PLUGINS_H
#define PLUGINS_H

#include "statics.h"
#include "utilities.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL char gszJSONPath[MAX_PATH];

GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();

GLOBAL typedef void(__cdecl *fPreGame__Start)(int *, bool *, int *, char *, bool *);
GLOBAL typedef void(__cdecl *fOnGame__Start)(int, bool, int, char *, bool);
GLOBAL typedef void(__cdecl *fPostGame__Start)(int);

GLOBAL typedef void(__cdecl *fPrePlayer_Entity__AddCollectible)(Player *, int *, int *);
GLOBAL typedef void(__cdecl *fOnPlayer_Entity__AddCollectible)(Player *, int, int);
GLOBAL typedef void(__cdecl *fPostPlayer_Entity__AddCollectible)(int);

GLOBAL typedef void(__cdecl *fPostTriggerBossDeath)(int);

GLOBAL typedef void(__cdecl *fOnLevel__Init)(int);
GLOBAL typedef void(__cdecl *fPostLevel__Init)(int);

GLOBAL typedef bool(__cdecl *fPreItemPool__GetCollectible)(int);
GLOBAL typedef void(__cdecl *fPostItemPool__GetCollectible)(int);

GLOBAL typedef bool(__cdecl *fPreItemPool__GetCard)(int);
GLOBAL typedef void(__cdecl *fPostItemPool__GetCard)(int);

GLOBAL typedef bool(__cdecl *fPreItemPool__GetRune)(int);
GLOBAL typedef void(__cdecl *fPostItemPool__GetRune)(int);

GLOBAL typedef void(__cdecl *fOnGameUpdate)();

GLOBAL typedef void(__cdecl *fOnReceiveMessage)(MessageMap);

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;

	fPreGame__Start PreGame__Start;
	fOnGame__Start OnGame__Start;
	fPostGame__Start PostGame__Start;

	fPrePlayer_Entity__AddCollectible PrePlayer_Entity__AddCollectible;
	fOnPlayer_Entity__AddCollectible OnPlayer_Entity__AddCollectible;
	fPostPlayer_Entity__AddCollectible PostPlayer_Entity__AddCollectible;

	fPostTriggerBossDeath PostTriggerBossDeath;

	fOnLevel__Init OnLevel__Init;
	fPostLevel__Init PostLevel__Init;

	fPreItemPool__GetCollectible PreItemPool__GetCollectible;
	fPostItemPool__GetCollectible PostItemPool__GetCollectible;

	fPreItemPool__GetCard PreItemPool__GetCard;
	fPostItemPool__GetCard PostItemPool__GetCard;

	fPreItemPool__GetRune PreItemPool__GetRune;
	fPostItemPool__GetRune PostItemPool__GetRune;

	fOnGameUpdate OnGameUpdate;

	fOnReceiveMessage OnReceiveMessage;

	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

GLOBAL PPLUGIN pPluginList;
GLOBAL PPLUGIN GetPluginByName(const char *name);
GLOBAL bool LoadPlugin(const char *);
GLOBAL bool UnloadPlugin(const char *);
GLOBAL void UnInitPlugins();
GLOBAL void InitPlugins();

GLOBAL void PreGame__Start(int *, bool *, int *, char *, bool *);
GLOBAL void OnGame__Start(int, bool, int, char *, bool);
GLOBAL void PostGame__Start(int);

GLOBAL void PrePlayer_Entity__AddCollectible(Player *, int *, int *);
GLOBAL void OnPlayer_Entity__AddCollectible(Player *, int, int);
GLOBAL void PostPlayer_Entity__AddCollectible(int);

GLOBAL void PostTriggerBossDeath(int);

GLOBAL void OnLevel__Init(int);
GLOBAL void PostLevel__Init(int);

GLOBAL bool PreItemPool__GetCollectible(int);
GLOBAL void PostItemPool__GetCollectible(int);

GLOBAL bool PreItemPool__GetCard(int);
GLOBAL void PostItemPool__GetCard(int);

GLOBAL bool PreItemPool__GetRune(int);
GLOBAL void PostItemPool__GetRune(int);

GLOBAL void OnGameUpdate();

GLOBAL void OnReceiveMessage(MessageMap);

#endif
