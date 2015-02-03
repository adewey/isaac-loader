#include "commands.h"
#include "utilities.h"
#include "hooks.h"
void InputCommand(int argc, char *argv[])
{
	/* handle commands */


}

FILE *originalStdOut = NULL;
FILE *originalStdIn = NULL;
void InitConsole()
{
	/* attach our console to our injector's console */
	AttachConsole(GetProcessId(L"injector.exe"));
	originalStdOut = freopen("CONOUT$", "w", stdout);
	originalStdIn = freopen("CONIN$", "r", stdin);
}

void RemoveConsole()
{
	freopen("CONOUT$", "w", originalStdOut);
	freopen("CONIN$", "r", originalStdIn);
	FreeConsole();
}

void ProcessCommand(char *buffer){
	if (strstr(buffer, "setkey ") != NULL){ //If substring 'setkey ' exists then
		cout << "Command Recognized. \n";
		char * keyLoc = strstr(buffer, "setkey ") + 7; // 7 is length of 'setkey '
		char *newline;
		if ((newline = strchr(keyLoc, '\n')) != NULL){
			*newline = '\0'; //Remove the newline character from the end of the buffer
		}
		SetTrackerID(keyLoc);
		cout << "Tracker ID set to: '" << GetTrackerID() << "';" << endl;
	}
	else{
		cout << "Not a command. \n";
	}
}
