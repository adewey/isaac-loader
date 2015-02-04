#ifndef PLUGIN_H
#define PLUGIN_H

#include "isaacdata.h"
#include "commands.h"
#include "plugins.h"

#define PAPI extern "C" __declspec(dllexport)

int APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	/* we really don't do anything here */
    return true;
}

#endif