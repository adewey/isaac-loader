#ifndef HOOKS_H
#define HOOKS_H

#include "utilities.h"

GLOBAL DWORD gdwBaseAddress;
GLOBAL DWORD gdwBaseSize;

void InitHooks();
void RemoveHooks();

#endif
