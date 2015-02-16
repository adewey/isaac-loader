#ifndef HOOKS_H
#define HOOKS_H
#include "Player.h"
#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;
GLOBAL DWORD gdwPlayerManager;
GLOBAL DWORD gdwGetPlayerEntity;
GLOBAL DWORD gdwGameUpdate;
GLOBAL DWORD gdwAddCollectible;
GLOBAL DWORD gdwSpawnEntity;

void InitHooks();
void RemoveHooks();
GLOBAL void AddCollectible(Player *,int);
#endif
