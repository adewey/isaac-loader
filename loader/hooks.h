#ifndef HOOKS_H
#define HOOKS_H

#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;
GLOBAL DWORD gdwGameUpdate;
GLOBAL DWORD gdwPlayer_Entity__AddCollectible;

void InitHooks();
void UnInitHooks();

#endif
