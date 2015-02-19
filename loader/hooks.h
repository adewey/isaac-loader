#ifndef HOOKS_H
#define HOOKS_H

#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;
GLOBAL DWORD gdwPlayerManager;
GLOBAL DWORD gdwGetPlayerEntity;
GLOBAL DWORD gdwGameUpdate;
GLOBAL DWORD gdwAddCollectible;
GLOBAL DWORD gdwSpawnEntity;
GLOBAL DWORD gdwFlashText;

void InitHooks();
void RemoveHooks();

#endif
