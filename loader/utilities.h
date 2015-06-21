#ifndef UTILITIES_H
#define UTILITIES_H

#define WIN32_LEAN_AND_MEAN
#define MAX_STRING 1024

#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include "dllmain.h"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

using namespace std;
using namespace rapidjson;

typedef map<string, string> MessageMap;

DWORD dwGetPidByName(TCHAR *wExeName);
DWORD dwGetModuleSize(char *);
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
GLOBAL DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

#endif
