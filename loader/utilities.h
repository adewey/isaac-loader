#ifndef UTILITIES_H
#define UTILITIES_H

#define WIN32_LEAN_AND_MEAN
#define MAX_STRING 1024
#include <windows.h>
#include "detours.h"
#include <iostream>
#include <map>
#include "dllmain.h"
using namespace std;

typedef map<string, string> MessageMap;

DWORD dwGetPidByName(TCHAR *wExeName);
DWORD dwGetModuleSize(char *);
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
GLOBAL DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

#endif
