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
