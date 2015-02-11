#ifndef PLUGINS_H
#define PLUGINS_H

#include "statics.h"
#include "utilities.h"

GLOBAL char gszPluginPath[MAX_PATH];
GLOBAL char gszINIPath[MAX_PATH];
GLOBAL typedef void(__cdecl *fInitPlugin)();
GLOBAL typedef void(__cdecl *fUnInitPlugin)();
GLOBAL typedef void(__cdecl *fOnAddCollectible)(Player *, int, int, int, int);
GLOBAL typedef void(__cdecl *fPreAddCollectible)(Player *, int *, int *, int *, int *);
GLOBAL typedef void(__cdecl *fPreSpawnEntity)(PointF *velocity, PointF *position, PPLAYERMANAGER *playerManager, int *entityID, int *variant, Entity *parent, int *subtype, unsigned int *seed);
GLOBAL typedef void(__cdecl *fOnSpawnEntity)(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, Entity *parent, int subtype, unsigned int seed);
GLOBAL typedef void(__cdecl *fOnGameUpdate)();

typedef struct _Plugin
{
	char szPluginName[MAX_PATH];
	HMODULE hModule;

	fInitPlugin InitPlugin;
	fUnInitPlugin UnInitPlugin;
	fOnAddCollectible OnAddCollectible;
	fPreAddCollectible PreAddCollectible;
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

GLOBAL void PreSpawnEntity(PointF *velocity, PointF *position, PPLAYERMANAGER *playerManager, int *entityID, int *variant, Entity *parent, int *subtype, unsigned int *seed);
GLOBAL void OnSpawnEntity(PointF *velocity, PointF *position, PPLAYERMANAGER playerManager, int entityID, int variant, Entity *parent, int subtype, unsigned int seed);

GLOBAL void OnGameUpdate();

#endif
