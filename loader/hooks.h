#ifndef HOOKS_H
#define HOOKS_H
#include "Player.h"
#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;
GLOBAL DWORD gdwPlayerManager;

void InitHooks();
void RemoveHooks();
GLOBAL void AddCollectible(Player *,int);
#endif
