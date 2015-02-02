#include "commands.h"
#include "utilities.h"

void InputCommand(int argc, char *argv[])
{
	/* handle commands */


}

FILE *originalStdOut = NULL;
void InitConsole()
{
	/* attach our console to our injector's console */
	AttachConsole(GetProcessId(L"injector.exe"));
	originalStdOut = freopen("CONOUT$", "w", stdout);
}

void RemoveConsole()
{
	freopen("CONOUT$", "w", originalStdOut);
	FreeConsole();
}
