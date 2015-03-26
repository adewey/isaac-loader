#include "commands.h"
#include "utilities.h"
PCOMMAND pCommandList = 0;

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i]) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			}
			else {
				_argv[j] = a;
				j++;
			}
		}
		else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

void ParseCommand(char *cpLine)
{
	if (cpLine[0] == '\n') return;
	PCHAR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvA(cpLine, &nArgs);
	/* pop our first argument (the command name) */
	char *Command = szArglist[0];
	szArglist = &szArglist[1];
	nArgs--;
	HandleCommand(Command, nArgs, szArglist);
}

void HandleCommand(char *Command, int argc, char *argv[])
{
	/* handle our command */
	PCOMMAND pCommand = pCommandList;
	while (pCommand)
	{
		if (_stricmp(Command, pCommand->Command) == 0)
			return pCommand->Function(argc, argv);
		pCommand = pCommand->pNext;
	}
}

FILE *originalStdOut = NULL;
FILE *originalStdIn = NULL;
void InitConsole()
{
	/* attach our console to our injector's console */
	AllocConsole();//AttachConsole(dwGetPidByName(L"injector.exe"));
	originalStdOut = freopen("CONOUT$", "w", stdout);
	originalStdIn = freopen("CONIN$", "r", stdin);
}

void UnInitConsole()
{
	freopen("CONOUT$", "w", originalStdOut);
	freopen("CONIN$", "r", originalStdIn);
	FreeConsole();
}

void AddCommand(char *Command, fCommand Function)
{
	PCOMMAND pCommand = new COMMAND;
	memset(pCommand, 0, sizeof(COMMAND));
	strncpy_s(pCommand->Command, Command, 64 - 1);
	pCommand->Function = Function;

	// perform insertion sort
	if (!pCommandList)
	{
		pCommandList = pCommand;
		return;
	}
	PCOMMAND pInsert = pCommandList;
	PCOMMAND pLast = 0;
	while (pInsert)
	{
		if (_stricmp(pCommand->Command, pInsert->Command) <= 0)
		{
			// insert here.
			if (pLast)
				pLast->pNext = pCommand;
			else
				pCommandList = pCommand;
			pCommand->pLast = pLast;
			pInsert->pLast = pCommand;
			pCommand->pNext = pInsert;
			return;
		}
		pLast = pInsert;
		pInsert = pInsert->pNext;
	}
	// End of list
	pLast->pNext = pCommand;
	pCommand->pLast = pLast;
}

bool RemoveCommand(char *Command)
{
	PCOMMAND pCommand = pCommandList;
	while (pCommand)
	{
		int Pos = _strnicmp(Command, pCommand->Command, 63);
		if (Pos<0)
		{
			return 0;
		}
		if (Pos == 0)
		{
			if (pCommand->pNext)
				pCommand->pNext->pLast = pCommand->pLast;
			if (pCommand->pLast)
				pCommand->pLast->pNext = pCommand->pNext;
			else
				pCommandList = pCommand->pNext;
			delete pCommand;
			return 1;
		}
		pCommand = pCommand->pNext;
	}
	return 0;
}
