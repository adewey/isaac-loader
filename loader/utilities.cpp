#include "utilities.h"
#include <Psapi.h>
#include <TlHelp32.h>
#include "detours.h"

using namespace std;

DWORD dwGetPidByName(TCHAR *wExeName)
{
	DWORD res = 0;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, wExeName) == 0)
			{
				res = entry.th32ProcessID;
				break;
			}
		}
	}
	CloseHandle(snapshot);
	return res;
}

GLOBAL DWORD dwGetModuleSize(char *szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandleA(szModule);
	if (hModule == 0) return 0;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo.SizeOfImage;
}

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}

GLOBAL DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (DWORD)(dwAddress + i);
	return 0;
}
