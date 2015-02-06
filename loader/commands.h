#ifndef COMMANDS_H
#define COMMANDS_H

#include "utilities.h"

typedef void (__cdecl *fCommand)(int argc, char *argv[]);

typedef struct _COMMAND {
	char Command[64];
	fCommand Function;
	struct _COMMAND* pLast;
	struct _COMMAND* pNext;
} COMMAND, *PCOMMAND;


GLOBAL PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc);
void ParseCommand(char *);
void HandleCommand(char *, int, char *[]);
void InitConsole();
void RemoveConsole();
GLOBAL void AddCommand(char *, fCommand);
GLOBAL bool RemoveCommand(char *);

#endif
