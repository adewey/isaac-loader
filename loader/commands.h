#ifndef COMMANDS_H
#define COMMANDS_H

#include "isaacdata.h"

typedef void (__cdecl *fCommand)(Player *pPlayer, int argc, char *argv[]);

typedef struct _COMMAND {
	char Command[64];
	fCommand Function;
	struct _COMMAND* pLast;
	struct _COMMAND* pNext;
} COMMAND, *PCOMMAND;


void ParseCommand(char *);
void HandleCommand(char *, int, char *[]);
void InitConsole();
void RemoveConsole();
void AddCommand(char *, fCommand);
bool RemoveCommand(char *);

#endif
