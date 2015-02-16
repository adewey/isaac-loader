#ifndef PLUGINS_H
#define PLUGINS_H

#include "statics.h"
#include "utilities.h"
#include "hooks.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL char gszINIPath[MAX_PATH];
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fPreAddCollectible)(Player *, int *, int *, int *, int *);
GLOBAL typedef void(__cdecl *fOnAddCollectible)(Player *, int, int, int, int);
GLOBAL typedef void(__cdecl *fPostAddCollectible)(int);
GLOBAL typedef void(__cdecl *fPreChangeKeys)(Player *, int *);
GLOBAL typedef void(__cdecl *fOnChangeKeys)(Player *, int);
GLOBAL typedef void(__cdecl *fPostChangeKeys)(int);
GLOBAL typedef void(__cdecl *fPreChangeBombs)(Player *, int *);
GLOBAL typedef void(__cdecl *fOnChangeBombs)(Player *, int);
GLOBAL typedef void(__cdecl *fPostChangeBombs)(int );
GLOBAL typedef void(__cdecl *fPreChangeCoins)(Player *, int *);
GLOBAL typedef void(__cdecl *fOnChangeCoins)(Player *, int);
GLOBAL typedef void(__cdecl *fPostChangeCoins)(int);
GLOBAL typedef void(__cdecl *fPreSpawnEntity)(PointF *velocity, PointF *position, PPLAYERMANAGER *playerManager, int *entityID, int *variant, Entity *parent, int *subtype, unsigned int *seed);
GLOBAL typedef void(__cdecl *fOnSpawnEntity)(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, Entity *parent, int subtype, unsigned int seed);
GLOBAL typedef void(__cdecl *fOnGameUpdate)();

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fPreAddCollectible PreAddCollectible;
	fOnAddCollectible OnAddCollectible;
	fPostAddCollectible PostAddCollectible;
	fPreChangeKeys PreChangeKeys;
	fOnChangeKeys OnChangeKeys;
	fPostChangeKeys PostChangeKeys;
	fPreChangeBombs PreChangeBombs;
	fOnChangeBombs OnChangeBombs;
	fPostChangeBombs PostChangeBombs;
	fPreChangeCoins PreChangeCoins;
	fOnChangeCoins OnChangeCoins;
	fPostChangeCoins PostChangeCoins;
	fPreSpawnEntity PreSpawnEntity;
	fOnSpawnEntity OnSpawnEntity;
	fOnGameUpdate OnGameUpdate;

	struct _Plugin* pLast;
	struct _Plugin* pNext;
} PLUGIN, *PPLUGIN;

GLOBAL PPLUGIN pPluginList;

GLOBAL bool LoadPlugin(const char *);
GLOBAL bool UnloadPlugin(const char *);
GLOBAL void UnloadPlugins();
GLOBAL void InitPlugins();

GLOBAL void PreAddCollectible(Player *, int *, int *, int *, int *);
GLOBAL void OnAddCollectible(Player *, int, int, int, int);
GLOBAL void PostAddCollectible(int);

GLOBAL void PreChangeKeys(Player *, int *);
GLOBAL void OnChangeKeys(Player *, int);
GLOBAL void PostChangeKeys(int);

GLOBAL void PreChangeBombs(Player *, int *);
GLOBAL void OnChangeBombs(Player *, int);
GLOBAL void PostChangeBombs(int);

GLOBAL void PreChangeCoins(Player *, int *);
GLOBAL void OnChangeCoins(Player *, int);
GLOBAL void PostChangeCoins(int);

GLOBAL void PreSpawnEntity(PointF *velocity, PointF *position, PPLAYERMANAGER *playerManager, int *entityID, int *variant, Entity *parent, int *subtype, unsigned int *seed);
GLOBAL void OnSpawnEntity(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, Entity *parent, int subtype, unsigned int seed);

GLOBAL void OnGameUpdate();

#endif
