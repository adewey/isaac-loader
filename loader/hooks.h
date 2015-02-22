#ifndef HOOKS_H
#define HOOKS_H

#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;
GLOBAL DWORD gdwGetPlayerEntity;
GLOBAL DWORD gdwGameUpdate;
GLOBAL DWORD gdwAddCollectible;
GLOBAL DWORD gdwPlayerManager;

void InitHooks();
void RemoveHooks();

#endif
