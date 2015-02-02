#ifndef UTILITIES_H
#define UTILITIES_H

#define WIN32_LEAN_AND_MEAN
#define MAX_STRING 1024
#include <windows.h>
#include <curl.h>
#include <tlhelp32.h>
#include "detours.h"

unsigned long GetModuleSize(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize);
unsigned long GetModuleBase(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize);
unsigned long GetProcessId(wchar_t* pszProcessName);
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);
PSTR GetNextArg(PCSTR szLine, DWORD dwNumber, BOOL CSV, CHAR Separator);
PSTR GetArg(PSTR szDest, PCSTR szSrc, DWORD dwNumber, BOOL LeaveQuotes, BOOL ToParen, BOOL CSV, CHAR Separator);

#endif
