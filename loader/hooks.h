#ifndef HOOKS_H
#define HOOKS_H

#include "isaacdata.h"

void InitHooks();
void RemoveHooks();
void SetTrackerID(char *);
char * GetTrackerID();
void ProcessCommand(char *);

#endif
