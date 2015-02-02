#include <curl.h>
#include "utilities.h"
#include <tlhelp32.h>
#include "detours.h"

using namespace std;

unsigned long GetModuleSize(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseSize;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}

unsigned long GetModuleBase(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseAddr;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}

unsigned long GetProcessId(wchar_t* pszProcessName)
{
	unsigned long dwResult = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
		if (Process32First(hSnapshot, &processEntry))
		{
			do
			{
				if (wcscmp(processEntry.szExeFile, pszProcessName) == 0)
				{
					dwResult = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &processEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (DWORD)(dwAddress + i);
	return 0;
}

PSTR GetNextArg(PCSTR szLine, DWORD dwNumber, BOOL CSV, CHAR Separator)
{
	PCSTR szNext = szLine;
	BOOL CustomSep = FALSE;
	BOOL InQuotes = FALSE;
	if (Separator != 0) CustomSep = TRUE;

	while (
		((!CustomSep) && (szNext[0] == ' '))
		|| ((!CustomSep) && (szNext[0] == '\t'))
		|| ((CustomSep) && (szNext[0] == Separator))
		|| ((!CustomSep) && (CSV) && (szNext[0] == ','))
		) szNext++;

	if ((INT)dwNumber < 1) return (PSTR)szNext;
	for (dwNumber; dwNumber>0; dwNumber--) {
		while ((
			((CustomSep) || (szNext[0] != ' '))
			&& ((CustomSep) || (szNext[0] != '\t'))
			&& ((!CustomSep) || (szNext[0] != Separator))
			&& ((CustomSep) || (!CSV) || (szNext[0] != ','))
			&& (szNext[0] != 0)
			)
			|| (InQuotes)
			) {
			if ((szNext[0] == 0) && (InQuotes)) {
				return (PSTR)szNext;
			}
			if (szNext[0] == '"') InQuotes = !InQuotes;
			szNext++;
		}
		while (
			((!CustomSep) && (szNext[0] == ' '))
			|| ((!CustomSep) && (szNext[0] == '\t'))
			|| ((CustomSep) && (szNext[0] == Separator))
			|| ((!CustomSep) && (CSV) && (szNext[0] == ','))
			) szNext++;
	}
	return (PSTR)szNext;
}

PSTR GetArg(PSTR szDest, PCSTR szSrc, DWORD dwNumber, BOOL LeaveQuotes, BOOL ToParen, BOOL CSV, CHAR Separator)
{
	DWORD i = 0;
	DWORD j = 0;
	BOOL CustomSep = FALSE;
	BOOL InQuotes = FALSE;
	PCSTR szTemp = szSrc;
	ZeroMemory(szDest, MAX_STRING);

	if (Separator != 0) CustomSep = TRUE;

	szTemp = GetNextArg(szTemp, dwNumber - 1, CSV, Separator);

	while ((
		((CustomSep) || (szTemp[i] != ' '))
		&& ((CustomSep) || (szTemp[i] != '\t'))
		&& ((CustomSep) || (!CSV) || (szTemp[i] != ','))
		&& ((!CustomSep) || (szTemp[i] != Separator))
		&& (szTemp[i] != 0)
		&& ((!ToParen) || (szTemp[i] != ')'))
		)
		|| (InQuotes)
		) {
		if ((szTemp[i] == 0) && (InQuotes)) {
			return szDest;
		}
		if (szTemp[i] == '"') {
			InQuotes = !InQuotes;
			if (LeaveQuotes) {
				szDest[j] = szTemp[i];
				j++;
			}
		}
		else {
			szDest[j] = szTemp[i];
			j++;
		}
		i++;
	}
	if ((ToParen) && (szTemp[i] == ')')) szDest[j] = ')';

	return szDest;
}