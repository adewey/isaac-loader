#ifndef UTILITIES_H
#define UTILITIES_H

#define WIN32_LEAN_AND_MEAN
#define MAX_STRING 1024
#include <windows.h>
#include "detours.h"
#include <iostream>
using namespace std;
#define GLOBAL extern "C" __declspec(dllexport)

DWORD dwGetModuleSize(char *);
DWORD dwGetPidByName(TCHAR *wExeName);
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

#endif
