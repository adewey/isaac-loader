#ifndef PLUGIN_H
#define PLUGIN_H

#define IMPORTS
#pragma warning(disable:4273)

#include "utilities.h"
#include "commands.h"
#include "plugins.h"
#include "hooks.h"
#define PAPI EXTERN_C __declspec(dllexport)

int APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	/* we really don't do anything here */
    return true;
}

#endif