#ifndef UTILITIES_H
#define UTILITIES_H

#define WIN32_LEAN_AND_MEAN
#define MAX_STRING 1024
#include <windows.h>
#include "detours.h"
#include <iostream>
#include "dllmain.h"
using namespace std;

DWORD dwGetPidByName(TCHAR *wExeName);
GLOBAL DWORD dwGetModuleSize(char *);
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
GLOBAL DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

#endif
