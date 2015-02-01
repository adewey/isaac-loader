#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>
/*
TODO(dither):
- Log errors so that we can debug if injection is failing
- Set our stream_key remotely
- Make this just something in the systray
*/

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

bool bInjectDLL(DWORD dwPid)
{
	/* we are assuming you compiled the files in debug mode the same as they are on github. this WILL change once we ship the product */
	PCHAR szTempPath;
	CHAR szPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szPath);
	szTempPath = strrchr(szPath, '\\');
	szTempPath[0] = '\0';
	strcat(szPath, "\\loader\\Debug\\loader.dll");
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!process) {
		printf("could not OpenProcess %d\n", dwPid);
		return false;
	}
	LPVOID loadlibrarya = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (!loadlibrarya) {
		CloseHandle(process);
		printf("cound not find LoadLibraryA\n");
		return false;
	}
	LPVOID allocatedmemory = (LPVOID)VirtualAllocEx(process, NULL, strlen(szPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocatedmemory) {
		printf("cound not VirtualAllocEx\n");
		CloseHandle(process);
		return false;
	}
	if (!WriteProcessMemory(process, allocatedmemory, szPath, strlen(szPath), NULL)) {
		CloseHandle(process);
		printf("cound not WriteProcessMemory\n");
		return false;
	}
	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibrarya, allocatedmemory, NULL, NULL);
	if (!threadID) {
		CloseHandle(process);
		printf("cound not CreateRemoteThread\n");
		return false;
	}
	CloseHandle(process);
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwPid = 0;
	bool injected = false;
	while (true)
	{
		dwPid = dwGetPidByName(L"isaac-ng.exe");
		if (dwPid && !injected)
		{
			/* let the game load a little before trying to inject */
			Sleep(3000);
			injected = bInjectDLL(dwPid);
		}
		else if (!dwPid && injected)
		{
			injected = !injected;
		}
		Sleep(5000);
	}
	return 0;
}

