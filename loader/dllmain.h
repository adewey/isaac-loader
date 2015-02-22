#ifndef IMPORTS
#define GLOBAL EXTERN_C __declspec(dllexport)
#else
#define GLOBAL EXTERN_C __declspec(dllimport)
#endif

#define IniReadString(section, key, szOut) GetPrivateProfileStringA(section, key, "", szOut, MAX_STRING, gszINIPath);
#define IniWriteString(section, key, szIn) WritePrivateProfileStringA(section, key, szIn, gszINIPath);

GLOBAL bool gbAttached;
